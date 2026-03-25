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

namespace HPHP {

// Entry point for the standalone sbcc-build binary.
// Called from sbcc-build main() after CLI parsing.
// configFiles and confStrings are loaded before hphp_process_init().
int sbcc_compiler_main(
    const std::string& outputPath,
    const std::string& sourceRoot,
    const std::string& fileListPath,
    int maxLocal,
    bool skipMissing,
    const std::vector<std::string>& configFiles = {},
    const std::vector<std::string>& confStrings = {});

} // namespace HPHP
