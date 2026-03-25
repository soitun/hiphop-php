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

///////////////////////////////////////////////////////////////////////////////
// SBCC Writer — streaming builder for .sbcc files.
//
// Uses Blob::Writer for the file container, following the same pattern as
// VirtualFileSystemWriter. UE blobs are written directly to the UE_BLOBS
// chunk during addUnit(). The hash map index and metadata are written at
// finish() time.
//
// Usage:
//   SBCCWriter writer(outputPath);
//   for each unit:
//     writer.addUnit(sha1, blobData, blobSize);
//   writer.finish();

struct SBCCWriter {
  explicit SBCCWriter(const std::string& outputPath);
  ~SBCCWriter();

  // Write one unit's serialized blob directly to the output file and
  // record compact metadata. blobData/blobSize is the output of
  // BlobEncoder after UnitEmitter::serde(). The caller may free
  // blobData after this call returns.
  void addUnit(const SHA1& sha1,
               const void* blobData,
               size_t blobSize);

  // Build the hash map index, write metadata, and finalize the file
  // (patches size table, atomic rename).
  void finish();

private:
  struct Data;
  std::unique_ptr<Data> m_data;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
