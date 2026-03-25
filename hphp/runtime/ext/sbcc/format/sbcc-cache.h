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

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace HPHP {

struct UnitEmitter;

///////////////////////////////////////////////////////////////////////////////

// Standalone Sandbox ByteCode Cache.
//
// Provides content-addressed pre-compiled bytecode for sandbox mode.
// Integrates with the unit compilation pipeline via registerUnitEmitterCacheLayer,
// creating the chain: SBCC -> BCCache -> HackC.
//
// The SBCC file uses a dedicated .sbcc container format (not .hhbc / RepoFile).
// SHA1-indexed lookup (content-addressed, like BCCache).

struct SBCCStats {
  int64_t hits{0};
  int64_t misses{0};
  int64_t corrupt{0};
  int64_t init_errors{0};
};

// Per-request counters, reset at the start of each request.
struct SBCCRequestStats {
  int64_t hits{0};
  int64_t misses{0};
  int64_t corrupt{0};
};

struct SandboxBytecodeCache {
  // Initialize from a .sbcc artifact. Throws on missing file or schema
  // mismatch. Called once during moduleInit(), single-threaded.
  static void init(const std::string& path);

  // Release resources.
  static void destroy();

  // Whether a cache is loaded.
  static bool has();

  // Register SBCC as a unit emitter cache layer via registerUnitEmitterCacheLayer.
  // Called from moduleLoad() only when Eval.SBCCPath is configured, so the dispatch
  // chain doesn't include SBCC on servers that don't use it.
  static void registerAsLayer();

  // Increment the vm.sbcc-init-errors global counter.
  static void incrementInitErrors();

  // Read current global (process-wide) counter values.
  static SBCCStats getStats();

  // Per-request stats — reset at requestInit, read during the request.
  static SBCCRequestStats getRequestStats();
  static void resetRequestStats();
  // Clear request-active flag at request end.
  static void onRequestShutdown();

  SandboxBytecodeCache() = delete;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
