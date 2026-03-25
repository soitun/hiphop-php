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

// sbcc-build: Compile source files to .sbcc.
//
// Calls compile_unit() from a local worker pool.
//
// Init model follows compiler/package.cpp::parseInit() — compile-only init
// with no request/session init and no systemlib loading.

#include "hphp/runtime/ext/sbcc/build/sbcc-compiler.h"

#include "hphp/runtime/ext/sbcc/build/sbcc-local.h"
#include "hphp/runtime/ext/sbcc/build/sbcc-file-list.h"

#include "hphp/hhvm/process-init.h"
#include "hphp/runtime/base/config.h"
#include "hphp/runtime/base/ini-setting.h"
#include "hphp/runtime/base/program-functions.h"
#include "hphp/runtime/base/runtime-option.h"
#include "hphp/runtime/base/unit-cache.h"
#include "hphp/runtime/vm/unit-parser.h"

#include "hphp/util/build-info.h"
#include "hphp/util/configs/eval.h"
#include "hphp/util/configs/jit.h"
#include "hphp/util/configs/server.h"
#include "hphp/util/hdf.h"
#include "hphp/util/logger.h"
#include "hphp/util/rds-local.h"

#include <filesystem>

namespace HPHP {

int sbcc_compiler_main(
    const std::string& outputPath,
    const std::string& sourceRoot,
    const std::string& fileListPath,
    int maxLocal,
    bool skipMissing,
    const std::vector<std::string>& configFiles,
    const std::vector<std::string>& confStrings) {
  if (outputPath.empty() || sourceRoot.empty() || fileListPath.empty()) {
    fprintf(stderr,
      "Usage: sbcc-build "
      "--output <path> --root <www> --file-list <list>\n");
    return 1;
  }

  try {
    // Normalize source root: ensure exactly one trailing slash.
    auto normalizedRoot = sourceRoot;
    while (!normalizedRoot.empty() && normalizedRoot.back() == '/') {
      normalizedRoot.pop_back();
    }
    normalizedRoot += '/';
    Cfg::Server::SourceRoot = normalizedRoot;

    // Config loading follows the compiler/package.cpp::parseInit() model:
    // rds::local::init() → RuntimeOption::Load() → register_process_init()
    // → hphp_process_init(true).
    rds::local::init();
    {
      IniSettingMap ini = IniSettingMap();
      Hdf config;
      for (const auto& filename : configFiles) {
        if (std::filesystem::exists(filename)) {
          Config::ParseConfigFile(filename, ini, config);
        } else {
          fprintf(stderr, "ERROR: Config file does not exist: %s\n",
                  filename.c_str());
          return 1;
        }
      }
      std::vector<std::string> messages;
      RuntimeOption::Load(ini, config, {}, confStrings, &messages);
      for (const auto& msg : messages) {
        Logger::Info("Config: %s", msg.c_str());
      }
    }

    Cfg::Jit::Enabled = false;
    register_process_init();
    hphp_process_init(true /* initForWorkerProcess */);

    // Don't use unit emitter caching in the builder.
    g_unit_emitter_cache_hook = nullptr;

    SCOPE_EXIT { hphp_process_exit(); };

    if (Cfg::Eval::EnableDecl) {
      Logger::Error("sbcc-build: incompatible with Eval.EnableDecl");
      return 1;
    }

    Logger::FInfo("SBCC compiler starting");
    Logger::FInfo("  Output:      {}", outputPath);
    Logger::FInfo("  Source root:  {}", normalizedRoot);
    Logger::FInfo("  File list:   {}", fileListPath);
    Logger::FInfo("  Schema:      {}", repoSchemaId());

    // Load file list.
    auto files = loadSBCCSourceFiles(normalizedRoot, fileListPath, skipMissing);
    if (files.empty()) {
      Logger::Error("No files to compile");
      return 1;
    }

    // Compile to .sbcc.
    SBCCCompileOptions opts;
    opts.sourceRoot = normalizedRoot;
    opts.outputPath = outputPath;
    opts.maxLocal = maxLocal;
    opts.skipMissing = skipMissing;

    return compileToSBCC(opts, files);

  } catch (const std::exception& e) {
    Logger::Error("SBCC compiler failed: %s", e.what());
    return 1;
  }
}

} // namespace HPHP
