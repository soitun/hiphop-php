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

#include "hphp/runtime/ext/sbcc/format/sbcc-cache.h"

#include "hphp/runtime/ext/sbcc/format/sbcc-reader.h"
#include "hphp/runtime/vm/unit-emitter.h"
#include "hphp/runtime/vm/unit-parser.h"

#include "hphp/util/logger.h"
#include "hphp/util/rds-local.h"
#include "hphp/util/service-data.h"
#include "hphp/util/trace.h"

namespace HPHP {

TRACE_SET_MOD(sbcc)

///////////////////////////////////////////////////////////////////////////////

namespace {

ServiceData::ExportedCounter* s_sbccHits =
  ServiceData::createCounter("vm.sbcc-hits");
ServiceData::ExportedCounter* s_sbccMisses =
  ServiceData::createCounter("vm.sbcc-misses");
ServiceData::ExportedCounter* s_sbccCorrupt =
  ServiceData::createCounter("vm.sbcc-corrupt");
ServiceData::ExportedCounter* s_sbccInitErrors =
  ServiceData::createCounter("vm.sbcc-init-errors");

RDS_LOCAL(SBCCRequestStats, s_sbcc_request_stats);

// True only while a request is active (set in resetRequestStats, cleared in
// onRequestShutdown). Used to guard per-request stat updates.
thread_local bool s_in_request = false;

SBCCReader s_reader;

std::unique_ptr<UnitEmitter> sbccCacheHook(
    const char* filename,
    const SHA1& sha1,
    folly::StringPiece::size_type fileLength,
    HhvmDeclProvider* provider,
    const std::function<std::unique_ptr<UnitEmitter>(bool)>& fallback) {

  // Guard: the layer is registered early (moduleLoad), but the reader may not
  // be initialized yet if init() hasn't run or failed.
  if (!s_reader.initialized()) return fallback(true);

  // Content-addressed lookup by mangled SHA1 (same strategy as BCCache).
  SBCCLookupResult lookupResult;
  auto ue = s_reader.lookup(sha1, filename, &lookupResult);

  switch (lookupResult) {
    case SBCCLookupResult::Hit:
      FTRACE(1, "SBCC HIT:   {}\n", filename);
      if (ue->m_ICE || ue->m_fatalUnit) {
        Logger::Error("SBCC: unexpected fatal/ICE unit in cache for %s "
                      "— falling back to compile", filename);
        s_sbccCorrupt->increment();
        if (s_in_request) s_sbcc_request_stats->corrupt++;
        return fallback(true);
      }
      s_sbccHits->increment();
      if (s_in_request) s_sbcc_request_stats->hits++;
      return ue;

    case SBCCLookupResult::Miss:
      s_sbccMisses->increment();
      if (s_in_request) s_sbcc_request_stats->misses++;
      FTRACE(1, "SBCC MISS:  {}\n", filename);
      break;

    case SBCCLookupResult::Corrupt:
      s_sbccMisses->increment();
      s_sbccCorrupt->increment();
      if (s_in_request) {
        s_sbcc_request_stats->misses++;
        s_sbcc_request_stats->corrupt++;
      }
      Logger::Error("SBCC corrupt cache entry for: %s", filename);
      break;
  }

  return fallback(true);
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

void SandboxBytecodeCache::init(const std::string& path) {
  s_reader.init(path);
  Logger::FInfo("SBCC: Loaded {} ({} entries)", path, s_reader.entryCount());
}

void SandboxBytecodeCache::destroy() {
  s_reader.destroy();
}

bool SandboxBytecodeCache::has() {
  return s_reader.initialized();
}

void SandboxBytecodeCache::registerAsLayer() {
  registerUnitEmitterCacheLayer("SBCC", UnitEmitterCacheHookPriority::LocalDisk, &sbccCacheHook);
}

void SandboxBytecodeCache::incrementInitErrors() {
  s_sbccInitErrors->increment();
}

SBCCStats SandboxBytecodeCache::getStats() {
  SBCCStats stats;
  stats.hits        = s_sbccHits->getValue();
  stats.misses      = s_sbccMisses->getValue();
  stats.corrupt     = s_sbccCorrupt->getValue();
  stats.init_errors = s_sbccInitErrors->getValue();
  return stats;
}

SBCCRequestStats SandboxBytecodeCache::getRequestStats() {
  if (!s_in_request) return SBCCRequestStats{};
  return *s_sbcc_request_stats;
}

void SandboxBytecodeCache::resetRequestStats() {
  s_in_request = true;
  *s_sbcc_request_stats = SBCCRequestStats{};
}

void SandboxBytecodeCache::onRequestShutdown() {
  *s_sbcc_request_stats = SBCCRequestStats{};
  s_in_request = false;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
