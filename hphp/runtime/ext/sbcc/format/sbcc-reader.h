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

#include "hphp/util/sha1.h"

namespace HPHP {

struct UnitEmitter;

///////////////////////////////////////////////////////////////////////////////
// SBCC lookup result classification.

enum class SBCCLookupResult : uint8_t {
  Hit,
  Miss,
  Corrupt,
};

///////////////////////////////////////////////////////////////////////////////
// SBCC Reader — uses Blob::Reader, following the VirtualFileSystem pattern.

struct SBCCReader {
  SBCCReader();
  ~SBCCReader();

  // Open the .sbcc file via Blob::Reader. Validates header.
  // Throws on error.
  void init(const std::string& path);

  // Release resources.
  void destroy();

  bool initialized() const;

  size_t entryCount() const;

  // Look up a unit by mangled SHA1 (content-addressed, like BCCache).
  // Returns: nullptr on miss, loaded UE on hit.
  // The returned UE's m_filepath is set to makeStaticString(filename).
  std::unique_ptr<UnitEmitter> lookup(
    const SHA1& sha1,
    const char* filename,
    SBCCLookupResult* result = nullptr) const;

private:
  struct Data;
  std::unique_ptr<Data> m_data;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
