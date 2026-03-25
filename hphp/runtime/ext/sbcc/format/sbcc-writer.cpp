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

#include "hphp/runtime/ext/sbcc/format/sbcc-writer.h"

#include "hphp/runtime/ext/sbcc/format/sbcc-file.h"

#include <cstring>

#include "hphp/util/blob-writer.h"
#include "hphp/util/logger.h"

namespace HPHP {

///////////////////////////////////////////////////////////////////////////////
// SBCCWriter — follows the VirtualFileSystemWriter pattern.

struct SBCCWriter::Data : Blob::Writer<SBCCChunk, SBCCIndex> {
  hphp_fast_string_map<Blob::Bounds> entries;
};

SBCCWriter::SBCCWriter(const std::string& outputPath)
  : m_data{std::make_unique<Data>()} {
  m_data->header(outputPath, {'S', 'B', 'C', 'C'}, kCurrentVersion);
}

SBCCWriter::~SBCCWriter() = default;

void SBCCWriter::addUnit(const SHA1& sha1,
                         const void* blobData,
                         size_t blobSize) {
  // Skip duplicate SHA1s — same content produces the same bytecode,
  // so the first entry is sufficient.
  if (m_data->entries.count(sha1.toString())) return;

  // Record the content offset before writing.
  auto contentOffset = m_data->sizes.get(SBCCChunk::UE_BLOBS);

  // Write blob directly to the UE_BLOBS chunk.
  m_data->write(SBCCChunk::UE_BLOBS, blobData, blobSize);

  // Record entry keyed by mangled SHA1 string.
  m_data->entries.emplace(
    sha1.toString(),
    Blob::Bounds{contentOffset, blobSize});
}

void SBCCWriter::finish() {
  // Write hash map index (SHA1 -> Blob::Bounds).
  m_data->hashMapIndex<Blob::Bounds, SBCCSha1Compare>(
    SBCCIndex::SHA1_TO_ENTRY, m_data->entries,
    [](auto const& it) { return it.first; },
    [](auto const& it) { return &it.second; });

  // Finalize: patch size table, atomic rename .part → final path.
  auto data = std::move(m_data);
  data->finish();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace HPHP
