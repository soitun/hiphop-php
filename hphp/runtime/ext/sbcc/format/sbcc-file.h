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

#include "hphp/util/blob.h"
#include "hphp/util/hash.h"
#include "hphp/util/sha1.h"

namespace HPHP {

///////////////////////////////////////////////////////////////////////////////
// SBCC file format constants and structures.
//
// Uses the Blob::Writer/Reader container format (hphp/util/blob-writer.h),
// following the same pattern as VirtualFileSystem (hphp/util/virtual-file-system).
// Compatibility is handled by Blob's built-in repo schema check.
//
// Layout (Blob container):
//   [Blob header: magic "SBCC" + version + repo schema + size table]
//   [Chunk UE_BLOBS:  concatenated serialized UnitEmitters]
//   [Index SHA1_TO_ENTRY: mangled SHA1 -> Blob::Bounds hash map]

constexpr uint16_t kCurrentVersion = 1;

// Chunks in the .sbcc Blob container.
enum class SBCCChunk : uint32_t {
  UE_BLOBS,
  SIZE,
};

// Blob hash map indexes.
enum class SBCCIndex : uint32_t {
  SHA1_TO_ENTRY,
  SIZE,
};

// Keyed by mangled SHA1 string in the SHA1_TO_ENTRY index.
// Key comparator for the Blob hash map index (SHA1 string comparison).
struct SBCCSha1Compare {
  bool equal(const std::string& s1, const std::string& s2) const {
    return s1 == s2;
  }
  size_t hash(const std::string& s) const {
    return hash_string_cs_software(s.c_str(), s.size());
  }
};

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
