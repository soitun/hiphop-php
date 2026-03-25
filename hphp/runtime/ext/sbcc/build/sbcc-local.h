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

#include <string>
#include <vector>

#include "hphp/runtime/ext/sbcc/build/sbcc-file-list.h"

namespace HPHP {

struct SBCCCompileOptions {
  std::string sourceRoot;
  std::string outputPath;
  int maxLocal{0};       // 0 = hardware_concurrency
  bool skipMissing{false};
};

// Compile source files to .sbcc using a local worker pool.
// Returns 0 on success, non-zero on error.
int compileToSBCC(
    const SBCCCompileOptions& options,
    const std::vector<SBCCSourceFile>& files);

} // namespace HPHP
