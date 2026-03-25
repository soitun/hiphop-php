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

// Standalone sbcc-build binary entry point.
//
// Usage:
//   sbcc-build -c <config.hdf> --output <path> --root <www>
//              --file-list <list> [--max-local N] [--skip-missing]
//
// The -c flag is CRITICAL: it loads config files (e.g. sandbox.hdf) so that
// RepoOptionsFlags::cacheKeySha1() matches between builder and runtime.
// Without it, config fingerprints will mismatch and all SBCC lookups miss.

#include "hphp/runtime/ext/sbcc/build/sbcc-compiler.h"

#include "hphp/runtime/base/type-string.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

struct Options {
  std::string outputPath;
  std::string sourceRoot;
  std::string fileListPath;
  int maxLocal{0};
  bool skipMissing{false};
  std::vector<std::string> configFiles;
  std::vector<std::string> confStrings;
};

void usage(const char* argv0) {
  fprintf(stderr,
    "Usage: %s -c <config.hdf> --output <path> --root <www>\n"
    "           --file-list <list> [--max-local N]\n"
    "           [--skip-missing] [-v Name=Value]\n",
    argv0);
}

bool parseArgs(int argc, char** argv, Options& opts) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
      opts.configFiles.push_back(argv[++i]);
    } else if ((arg == "-v" || arg == "--config-value") && i + 1 < argc) {
      opts.confStrings.push_back(argv[++i]);
    } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
      opts.outputPath = argv[++i];
    } else if (arg == "--root" && i + 1 < argc) {
      opts.sourceRoot = argv[++i];
    } else if (arg == "--file-list" && i + 1 < argc) {
      opts.fileListPath = argv[++i];
    } else if (arg == "--max-local" && i + 1 < argc) {
      const char* val = argv[++i];
      char* end = nullptr;
      long v = std::strtol(val, &end, 10);
      if (end == val || *end != '\0' || v < 0 || v > INT_MAX) {
        fprintf(stderr,
          "WARNING: Invalid --max-local value '%s', using default "
          "(hardware concurrency)\n", val);
        opts.maxLocal = 0;
      } else {
        opts.maxLocal = static_cast<int>(v);
      }
    } else if (arg == "--skip-missing") {
      opts.skipMissing = true;
    } else if (arg == "--help" || arg == "-h") {
      return false;
    } else {
      fprintf(stderr, "Unknown argument: %s\n", arg.c_str());
      return false;
    }
  }
  return !opts.outputPath.empty() && !opts.sourceRoot.empty() &&
         !opts.fileListPath.empty();
}

} // namespace

int main(int argc, char** argv) {
  HPHP::StaticString::CreateAll();

  Options opts;
  if (!parseArgs(argc, argv, opts)) {
    usage(argv[0]);
    return 1;
  }

  if (opts.configFiles.empty() && opts.confStrings.empty()) {
    fprintf(stderr,
      "WARNING: No -c or -v config specified. Artifacts may miss at "
      "runtime due to config fingerprint mismatch.\n");
  }

  return HPHP::sbcc_compiler_main(
    opts.outputPath, opts.sourceRoot, opts.fileListPath,
    opts.maxLocal, opts.skipMissing,
    opts.configFiles, opts.confStrings);
}
