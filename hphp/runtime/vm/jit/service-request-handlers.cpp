/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/runtime/vm/jit/service-request-handlers.h"

#include "hphp/runtime/ext/asio/ext_static-wait-handle.h"

#include "hphp/runtime/vm/jit/code-cache.h"
#include "hphp/runtime/vm/jit/jit-resume-addr-defs.h"
#include "hphp/runtime/vm/jit/mcgen.h"
#include "hphp/runtime/vm/jit/mcgen-async.h"
#include "hphp/runtime/vm/jit/perf-counters.h"
#include "hphp/runtime/vm/jit/tc.h"
#include "hphp/runtime/vm/jit/tc-internal.h"
#include "hphp/runtime/vm/jit/tc-record.h"
#include "hphp/runtime/vm/jit/translator-inline.h"
#include "hphp/runtime/vm/jit/unwind-itanium.h"
#include "hphp/runtime/vm/jit/write-lease.h"

#include "hphp/runtime/vm/bytecode.h"
#include "hphp/runtime/vm/hhbc.h"
#include "hphp/runtime/vm/resumable.h"
#include "hphp/runtime/vm/runtime.h"
#include "hphp/runtime/vm/workload-stats.h"

#include "hphp/util/configs/debugger.h"
#include "hphp/util/ringbuffer.h"
#include "hphp/util/trace.h"

TRACE_SET_MOD(mcg)

namespace HPHP::jit::svcreq {

namespace {

///////////////////////////////////////////////////////////////////////////////

RegionContext getContext(SrcKey sk, bool profiling) {
  RegionContext ctx { sk, liveSpOff() };

  auto const func = sk.func();
  auto const fp = vmfp();
  auto const sp = vmsp();

  always_assert(func == fp->func());
  auto const ctxClass = func->cls();
  auto const addLiveType = [&](Location loc, tv_rval tv) {
    auto const type = typeFromTV(tv, ctxClass);
    ctx.liveTypes.push_back({loc, type});

    FTRACE(2, "Added live type: {}\n", show(ctx.liveTypes.back()));
  };

  // Track local types.
  auto const numInitedLocals = sk.funcEntry()
    ? func->numFuncEntryInputs() : func->numLocals();
  for (uint32_t i = 0; i < numInitedLocals; ++i) {
    addLiveType(Location::Local{i}, frame_local(fp, i));
  }

  // Track stack types.
  int32_t stackOff = 0;
  visitStackElems(
    fp, sp,
    [&] (const TypedValue* tv) {
      addLiveType(Location::Stack{ctx.spOffset - stackOff}, tv);
      stackOff++;
    }
  );

  // Track the mbase type.  The member base register is only valid after a
  // member base op and before a member final op---and only AssertRAT*'s are
  // allowed to intervene in a sequence of bytecode member operations.
  if (!sk.funcEntry()) {
    // Get the bytecode for `ctx', skipping Asserts.
    auto const op = [&] {
      auto pc = func->at(sk.offset());
      while (isTypeAssert(peek_op(pc))) {
        pc += instrLen(pc);
      }
      return peek_op(pc);
    }();
    assertx(!isTypeAssert(op));

    if (isMemberDimOp(op) || isMemberFinalOp(op)) {
      auto const mbase = vmMInstrState().base;
      assertx(mbase != nullptr);
      addLiveType(Location::MBase{}, mbase);
    }
  }

  return ctx;
}

/*
 * Create a translation for the SrcKey specified in args.
 *
 * If a translation for this SrcKey already exists it will be returned. The kind
 * of translation created will be selected based on the SrcKey specified.
 */
TranslationResult getTranslation(SrcKey sk) {
  sk.func()->validate();

  if (!RID().getJit()) {
    SKTRACE(2, sk, "punting because jit was disabled\n");
    return TranslationResult::failTransiently();
  }

  if (auto const sr = tc::findSrcRec(sk)) {
    if (auto const tca = sr->getTopTranslation()) {
      SKTRACE(2, sk, "getTranslation: found %p\n", tca);
      return TranslationResult{tca};
    }
  }

  if (UNLIKELY(RID().isJittingDisabled())) {
    SKTRACE(2, sk, "punting because jitting code was disabled\n");
    return TranslationResult::failTransiently();
  }

  if (UNLIKELY(Cfg::Debugger::EnableVSDebugger && Cfg::Eval::EmitDebuggerIntrCheck)) {
    assertx(!Cfg::Repo::Authoritative);
    sk.func()->ensureDebuggerIntrSetLinkBound();
  }

  if (UNLIKELY(!Cfg::Repo::Authoritative && sk.unit()->isCoverageEnabled())) {
    assertx(Cfg::Eval::EnablePerFileCoverage);
    SKTRACE(2, sk, "punting because per file code coverage is enabled\n");
    return TranslationResult::failTransiently();
  }

  auto args = TransArgs{sk};
  args.kind = tc::profileFunc(args.sk.func()) ?
    TransKind::Profile : TransKind::Live;

  if (auto const s = tc::shouldTranslate(args.sk, args.kind);
      s != TranslationResult::Scope::Success) {
    return TranslationResult{s};
  }

  auto const ctx = getContext(args.sk, args.kind == TransKind::Profile);
  if (mcgen::isAsyncJitEnabled(args.kind)) {
    assertx(isLive(args.kind));
    mcgen::enqueueAsyncTranslateRequest(ctx, 0);
    return TranslationResult::failTransiently();
  }

  LeaseHolder writer(sk.func(), args.kind);
  if (!writer) return TranslationResult::failTransiently();

  if (auto const s = tc::shouldTranslate(args.sk, args.kind);
      s != TranslationResult::Scope::Success) {
    return TranslationResult{s};
  }

  if (tc::createSrcRec(sk, liveSpOff()) == nullptr) {
    // ran out of TC space
    return TranslationResult::failForProcess();
  }

  if (auto const tca = tc::findSrcRec(sk)->getTopTranslation()) {
    // Handle extremely unlikely race; someone may have just added the first
    // translation for this SrcRec while we did a non-blocking wait on the
    // write lease in createSrcRec().
    return TranslationResult{tca};
  }

  return mcgen::retranslate(args, ctx);
}

///////////////////////////////////////////////////////////////////////////////

}

namespace {

TCA resume(SrcKey sk, TranslationResult transResult) noexcept {
  auto const start = [&] {
    if (auto const addr = transResult.addr()) return addr;
    if (sk.funcEntry()) {
      sk.advance();
      vmpc() = sk.pc();
      return transResult.scope() == TranslationResult::Scope::Transient
        ? tc::ustubs().interpHelperFuncEntryFromTC
        : tc::ustubs().interpHelperNoTranslateFuncEntryFromTC;
    }
    vmpc() = sk.pc();
    return transResult.scope() == TranslationResult::Scope::Transient
      ? tc::ustubs().interpHelperFromTC
      : tc::ustubs().interpHelperNoTranslateFromTC;
  }();

  if (Trace::moduleEnabled(Trace::ringbuffer, 1)) {
    auto skData = sk.valid() ? sk.toAtomicInt() : uint64_t(-1LL);
    Trace::ringbufferEntry(Trace::RBTypeResumeTC, skData, (uint64_t)start);
  }

  regState() = VMRegState::DIRTY;
  return start;
}

void syncRegs(SBInvOffset spOff) noexcept {
  assert_native_stack_aligned();

  // This is a lie, only vmfp() is synced. We will sync vmsp() below and vmpc()
  // later if we are going to use the resume helper.
  regState() = VMRegState::CLEAN;
  vmsp() = Stack::anyFrameStackBase(vmfp()) - spOff.offset;
  vmJitReturnAddr() = nullptr;
}

}

void uninitDefaultArgs(ActRec* fp, uint32_t numEntryArgs,
                       uint32_t numNonVariadicParams) noexcept {
  // JIT may optimize away uninit writes for default arguments. Write them, as
  // we may inspect them or continue execution in the interpreter.
  for (auto param = numEntryArgs; param < numNonVariadicParams; ++param) {
    tvWriteUninit(frame_local(fp, param));
  }
}

TCA handleTranslate(Offset bcOff, SBInvOffset spOff) noexcept {
  syncRegs(spOff);
  FTRACE(1, "handleTranslate {}\n", vmfp()->func()->fullName()->data());

  auto const sk = SrcKey { liveFunc(), bcOff, liveResumeMode() };
  return resume(sk, getTranslation(sk));
}

TCA handleTranslateFuncEntry(uint32_t numArgs) noexcept {
  syncRegs(SBInvOffset{0});
  uninitDefaultArgs(vmfp(), numArgs, liveFunc()->numNonVariadicParams());
  FTRACE(1, "handleTranslateFuncEntry {}\n",
         vmfp()->func()->fullName()->data());

  auto const sk = SrcKey { liveFunc(), numArgs, SrcKey::FuncEntryTag {} };
  return resume(sk, getTranslation(sk));
}

TCA handleTranslateMainFuncEntry() noexcept {
    syncRegs(SBInvOffset{0});
    FTRACE(1, "handleTranslateMainFuncEntry {}\n",
           vmfp()->func()->fullName()->data());
    auto const numArgs = liveFunc()->numNonVariadicParams();
    auto const sk = SrcKey { liveFunc(), numArgs, SrcKey::FuncEntryTag {} };
    return resume(sk, getTranslation(sk));
}

TranslationResult::Scope shouldEnqueueForRetranslate(
  const RegionContext& context) {
  return tc::shouldTranslate(context.sk, TransKind::Live);
}

TCA handleRetranslate(Offset bcOff, SBInvOffset spOff) noexcept {
  syncRegs(spOff);
  FTRACE(1, "handleRetranslate {}\n", vmfp()->func()->fullName()->data());

  INC_TPC(retranslate);
  auto const sk = SrcKey { liveFunc(), bcOff, liveResumeMode() };
  auto const isProfile = tc::profileFunc(sk.func());
  auto const kind = isProfile ? TransKind::Profile : TransKind::Live;
  auto const context = getContext(sk, isProfile);
  if (mcgen::isAsyncJitEnabled(kind)) {
    assertx(isLive(kind));
    auto const res = shouldEnqueueForRetranslate(context);
    if (res != TranslationResult::Scope::Success) {
      FTRACE_MOD(Trace::async_jit, 2,
                 "shouldEnqueueForRetranslate failed for sk {}\n", show(sk));
      return resume(sk, TranslationResult{res});
    }
    auto const srcRec = tc::findSrcRec(sk);
    assertx(srcRec);
    auto const currNumTrans = srcRec->numTrans();
    FTRACE_MOD(Trace::async_jit, 2,
               "Enqueueing sk {} from handleRetranslate\n",
               show(context.sk));
    mcgen::enqueueAsyncTranslateRequest(context, currNumTrans);
    return resume(sk, TranslationResult::failTransiently());
  }
  auto const transResult = mcgen::retranslate(TransArgs{sk}, context);
  SKTRACE(2, sk, "retranslated @%p\n", transResult.addr());
  return resume(sk, transResult);
}

TCA handleRetranslateFuncEntry(uint32_t numArgs) noexcept {
  syncRegs(SBInvOffset{0});
  uninitDefaultArgs(vmfp(), numArgs, liveFunc()->numNonVariadicParams());
  FTRACE(1, "handleRetranslateFuncEntry {}\n",
         vmfp()->func()->fullName()->data());

  INC_TPC(retranslate);
  auto const sk = SrcKey { liveFunc(), numArgs, SrcKey::FuncEntryTag {} };
  auto const isProfile = tc::profileFunc(sk.func());
  auto const kind = isProfile ? TransKind::Profile : TransKind::Live;
  auto const context = getContext(sk, isProfile);
  if (mcgen::isAsyncJitEnabled(kind)) {
    assertx(isLive(kind));
    auto const res = shouldEnqueueForRetranslate(context);
    if (res != TranslationResult::Scope::Success) {
      FTRACE_MOD(Trace::async_jit, 2,
                 "shouldEnqueueForRetranslate failed for sk {}\n", show(sk));
      return resume(sk, TranslationResult{res});
    }
    auto const srcRec = tc::findSrcRec(sk);
    assertx(srcRec);
    auto const currNumTrans = srcRec->numTrans();
    FTRACE_MOD(Trace::async_jit, 2,
               "Enqueueing sk {} from handleRetranslateFuncEntry\n",
               show(context.sk));
    mcgen::enqueueAsyncTranslateRequest(context, currNumTrans);
    return resume(sk, TranslationResult::failTransiently());
  }
  auto const transResult = mcgen::retranslate(TransArgs{sk}, context);
  SKTRACE(2, sk, "retranslated @%p\n", transResult.addr());
  return resume(sk, transResult);
}

TCA handleRetranslateOpt(uint32_t numArgs) noexcept {
  syncRegs(SBInvOffset{0});
  uninitDefaultArgs(vmfp(), numArgs, liveFunc()->numNonVariadicParams());
  FTRACE(1, "handleRetranslateOpt {}\n", vmfp()->func()->fullName()->data());

  auto const sk = SrcKey { liveFunc(), numArgs, SrcKey::FuncEntryTag {} };
  auto const translated = mcgen::retranslateOpt(sk.funcID());
  vmpc() = sk.advanced().pc();
  regState() = VMRegState::DIRTY;

  if (translated) {
    // Retranslation was successful. Resume execution at the new optimized
    // translation.
    return tc::ustubs().resumeHelperFuncEntryFromTC;
  } else {
    // Retranslation failed, probably because we couldn't get the write
    // lease. Interpret a BB before running more Profile translations, to
    // avoid spinning through this path repeatedly.
    return tc::ustubs().interpHelperFuncEntryFromTC;
  }
}

TCA handlePostInterpRet(uint32_t callOffAndFlags) noexcept {
  assert_native_stack_aligned();
  regState() = VMRegState::CLEAN; // partially a lie: vmpc() isn't synced

  auto const callOffset = callOffAndFlags >> ActRec::CallOffsetStart;
  auto const isAER = callOffAndFlags & (1 << ActRec::AsyncEagerRet);

  FTRACE(3, "handlePostInterpRet to {}@{}{}\n",
         vmfp()->func()->fullName()->data(), callOffset, isAER ? "a" : "");

  // Reconstruct PC so that the subsequent logic have clean reg state.
  vmpc() = skipCall(vmfp()->func()->at(callOffset));
  vmJitReturnAddr() = nullptr;

  if (isAER) {
    // When returning to the interpreted FCall, the execution continues at
    // the next opcode, not honoring the request for async eager return.
    // If the callee returned eagerly, we need to wrap the result into
    // StaticWaitHandle.
    assertx(tvIsPlausible(vmsp()[0]));
    assertx(vmsp()[0].m_aux.u_asyncEagerReturnFlag + 1 < 2);
    if (vmsp()[0].m_aux.u_asyncEagerReturnFlag) {
      auto const waitHandle = c_StaticWaitHandle::CreateSucceeded(vmsp()[0]);
      vmsp()[0] = make_tv<KindOfObject>(waitHandle);
    }
  }

  auto const sk = liveSK();
  return resume(sk, getTranslation(sk));
}

TCA handleBindCall(TCA toSmash, Func* func, int32_t numArgs) {
  TRACE(2, "bindCall %s, numArgs %d\n", func->fullName()->data(), numArgs);
  auto const trans = mcgen::getFuncPrologue(func, numArgs);
  TRACE(2, "bindCall immutably %s -> %p\n",
        func->fullName()->data(), trans.addr());

  if (trans.isProcessPersistentFailure()) {
    // We can't get a translation for this and we can't create any new
    // ones any longer. Smash the call site with a stub which will
    // interp the prologue, then run resumeHelperNoTranslateFromInterp.
    tc::bindCall(
      toSmash,
      tc::ustubs().fcallHelperNoTranslateThunk,
      func,
      numArgs
    );
    return tc::ustubs().fcallHelperNoTranslateThunk;
  } else if (trans.addr()) {
    // Using start is racy but bindCall will recheck the start address after
    // acquiring a lock on the ProfTransRec
    tc::bindCall(toSmash, trans.addr(), func, numArgs);
    return trans.addr();
  } else {
    // We couldn't get a prologue address. Return a stub that will finish
    // entering the callee frame in C++, then call handleResume at the callee's
    // entry point.
    return trans.isRequestPersistentFailure()
      ? tc::ustubs().fcallHelperNoTranslateThunk
      : tc::ustubs().fcallHelperThunk;
  }
}

std::string ResumeFlags::show() const {
  std::vector<std::string> flags;
  if (m_noTranslate) flags.emplace_back("noTranslate");
  if (m_interpFirst) flags.emplace_back("interpFirst");
  if (m_funcEntry) flags.emplace_back("funcEntry");
  return folly::join(", ", flags);
}

namespace {
void logResume(SrcKey sk, ResumeFlags flags) {
  if (!StructuredLog::coinflip(Cfg::Eval::HandleResumeStatsRate)) return;

  StructuredLogEntry ent;

  std::set<folly::StringPiece> flgs;
  if (flags.m_noTranslate) flgs.emplace("noTranslate");
  if (flags.m_interpFirst) flgs.emplace("interpFirst");
  if (flags.m_funcEntry)   flgs.emplace("funcEntry");
  if (!flgs.empty())       ent.setSet("flags", flgs);

  // Canonicalize path
  std::filesystem::path fpath = sk.unit()->filepath()->toCppString();
  auto const dir = RepoOptions::forFile(fpath.c_str()).dir();
  auto const rel = std::filesystem::proximate(fpath, dir);
  auto const fn = folly::cEscape<std::string>(sk.func()->fullName()->slice());

  ent.setInt("sample_rate", Cfg::Eval::HandleResumeStatsRate);
  ent.setStr("file", rel.native());
  ent.setInt("line", sk.lineNumber());
  ent.setInt("offset", sk.offset());
  ent.setStr("func", sk.func()->name()->slice());
  if (sk.func()->isMethod()) {
    if (auto c = sk.func()->cls()) ent.setStr("class", c->name()->slice());
    else ent.setStr("class", sk.func()->preClass()->name()->slice());
  }
  ent.setStr("unit_sha1", sk.unit()->sha1().toString());
  ent.setStr("full_func_name", fn);
  ent.setInt("prologue", sk.prologue() ? 1 : 0);
  ent.setInt("funcEntry", sk.funcEntry() ? 1 : 0);
  ent.setInt("hasThis", sk.hasThis() ? 1 : 0);
  switch (sk.resumeMode()) {
  case ResumeMode::None: ent.setStr("resume_mode", "None");       break;
  case ResumeMode::Async: ent.setStr("resume_mode", "Async");     break;
  case ResumeMode::GenIter: ent.setStr("resume_mode", "GenIter"); break;
  }
  if (sk.prologue() || sk.funcEntry()) {
    ent.setInt("num_entry_args", sk.numEntryArgs());
  }
  ent.setStr("inst", sk.showInst());

  ent.setInt("main_used_bytes", tc::code().main().used());
  ent.setInt("cold_used_bytes", tc::code().cold().used());
  ent.setInt("frozen_used_bytes", tc::code().frozen().used());
  ent.setInt("data_used_bytes", tc::code().data().used());

  if (auto const sr = tc::findSrcRec(sk)) {
    auto const lock = sr->readlock();
    ent.setInt("src_rec", 1);
    ent.setInt("num_trans", sr->translations().size());
    ent.setInt("tail_jumps", sr->tailFallbackJumps().size());
    ent.setInt("incoming_branches", sr->incomingBranches().size());
    ent.setInt("max_trans", Cfg::Jit::MaxTranslations);
  } else {
    ent.setInt("src_rec", 0);
  }

  ent.setInt("can_translate", tc::canTranslate() ? 1 : 0);
  ent.setInt("profile_count", sk.func()->readJitReqCount());
  ent.setInt("live_threshold", Cfg::Jit::LiveThreshold);
  ent.setInt("live_main_usage", tc::getLiveMainUsage());
  ent.setInt("live_main_limit", Cfg::Jit::MaxLiveMainUsage);

  StructuredLog::log("hhvm_resume_locations", ent);
}
}

JitResumeAddr handleResume(ResumeFlags flags) {
  assert_native_stack_aligned();
  FTRACE(1, "handleResume({})\n", flags.show());

  regState() = VMRegState::CLEAN;
  assertx(vmpc());

  auto sk = liveSK();
  if (flags.m_funcEntry) {
    assertx(sk.resumeMode() == ResumeMode::None);
    auto const func = sk.func();
    auto numArgs = func->numNonVariadicParams();
    while (numArgs > 0 && !frame_local(vmfp(), numArgs - 1)->is_init()) {
      --numArgs;
    }
    DEBUG_ONLY auto const entryOffset = sk.offset();
    sk = SrcKey { sk.func(), numArgs, SrcKey::FuncEntryTag {} };
    assertx(sk.entryOffset() == entryOffset);

    vmsp() = Stack::frameStackBase(vmfp());
  }
  FTRACE(2, "handleResume: sk: {}\n", showShort(sk));

  auto const findOrTranslate = [&] (ResumeFlags flags) -> JitResumeAddr {
    if (!RID().getJit()) return JitResumeAddr::none();

    if (!flags.m_noTranslate) {
      auto const trans = getTranslation(sk);
      if (auto const addr = trans.addr()) {
        if (sk.funcEntry()) return JitResumeAddr::transFuncEntry(addr);
        return JitResumeAddr::trans(addr);
      }
      if (!trans.isRequestPersistentFailure()) return JitResumeAddr::none();
      return JitResumeAddr::helper(
        sk.funcEntry()
          ? tc::ustubs().interpHelperNoTranslateFuncEntryFromInterp
          : tc::ustubs().interpHelperNoTranslateFromInterp
      );
    }

    if (auto const sr = tc::findSrcRec(sk)) {
      if (auto const tca = sr->getTopTranslation()) {
        SKTRACE(2, sk, "handleResume: found %p\n", tca);
        if (sk.funcEntry()) return JitResumeAddr::transFuncEntry(tca);
        return JitResumeAddr::trans(tca);
      }
    }
    return JitResumeAddr::none();
  };

  auto start = JitResumeAddr::none();
  if (!flags.m_interpFirst) start = findOrTranslate(flags);
  if (!flags.m_noTranslate && flags.m_interpFirst) INC_TPC(interp_bb_force);

  vmJitReturnAddr() = nullptr;
  vmJitCalledFrame() = vmfp();
  SCOPE_EXIT { vmJitCalledFrame() = nullptr; };

  // If we can't get a translation at the current SrcKey, interpret basic
  // blocks until we end up somewhere with a translation (which we may have
  // created, if the lease holder dropped it).
  if (!start) {
    WorkloadStats guard(WorkloadStats::InInterp);
    tracing::BlockNoTrace _{"dispatch-bb"};

    // Log the resume event to scuba
    logResume(sk, flags);

    if (sk.funcEntry()) {
      auto const savedRip = vmfp()->m_savedRip;
      if (!funcEntry()) {
        FTRACE(2, "handleResume: returning to rip={} pc={} after intercept\n",
               TCA(savedRip), vmpc());
        // The callee was intercepted and should be skipped. In that case,
        // return to the caller. If we entered this frame from the interpreter,
        // use the resumeHelper, as the retHelper logic has been already
        // performed and the frame has been overwritten by the return value.
        regState() = VMRegState::DIRTY;
        if (isReturnHelper(savedRip)) {
          return
            JitResumeAddr::helper(jit::tc::ustubs().resumeHelperFromInterp);
        }
        return JitResumeAddr::ret(TCA(savedRip));
      }
      sk.advance();
    }

    // If background jit is enabled, enqueueing a new translation request
    // after every basic block may generate too many overlapping translations
    // because no thread takes a function-level lease.
    auto const kind = tc::profileFunc(sk.func()) ?
      TransKind::Profile : TransKind::Live;
    if (mcgen::isAsyncJitEnabled(kind)) flags = flags.noTranslate();
    do {
      INC_TPC(interp_bb);
      if (auto const retAddr = HPHP::dispatchBB()) {
        start = retAddr;
      } else {
        assertx(vmpc());
        sk = liveSK();
        start = findOrTranslate(flags);
      }
    } while (!start);
  }

  if (Trace::moduleEnabled(Trace::ringbuffer, 1)) {
    auto skData = sk.valid() ? sk.toAtomicInt() : uint64_t(-1LL);
    Trace::ringbufferEntry(Trace::RBTypeResumeTC, skData,
                           (uint64_t)start.handler);
  }

  regState() = VMRegState::DIRTY;
  return start;
}

///////////////////////////////////////////////////////////////////////////////

}
