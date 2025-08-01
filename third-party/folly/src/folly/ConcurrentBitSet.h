/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <limits>

#include <folly/Portability.h>

namespace folly {

/**
 * An atomic bitset of fixed size (specified at compile time).
 *
 * Formerly known as AtomicBitSet. It was renamed while fixing a bug
 * to avoid any silent breakages during run time.
 */
template <size_t N>
class ConcurrentBitSet {
 public:
  /**
   * Construct a ConcurrentBitSet; all bits are initially false.
   */
  ConcurrentBitSet();

  ConcurrentBitSet(const ConcurrentBitSet&) = delete;
  ConcurrentBitSet& operator=(const ConcurrentBitSet&) = delete;

  /**
   * Set bit idx to true, using the given memory order. Returns the
   * previous value of the bit.
   *
   * Note that the operation is a read-modify-write operation due to the use
   * of fetch_or.
   */
  bool set(size_t idx, std::memory_order order = std::memory_order_seq_cst);

  /**
   * Set bit idx to false, using the given memory order. Returns the
   * previous value of the bit.
   *
   * Note that the operation is a read-modify-write operation due to the use
   * of fetch_and.
   */
  bool reset(size_t idx, std::memory_order order = std::memory_order_seq_cst);

  /**
   * Set bit idx to the given value, using the given memory order. Returns
   * the previous value of the bit.
   *
   * Note that the operation is a read-modify-write operation due to the use
   * of fetch_and or fetch_or.
   *
   * Yes, this is an overload of set(), to keep as close to std::bitset's
   * interface as possible.
   */
  bool set(
      size_t idx,
      bool value,
      std::memory_order order = std::memory_order_seq_cst);

  /**
   * Read bit idx.
   */
  bool test(
      size_t idx, std::memory_order order = std::memory_order_seq_cst) const;

  /**
   * Same as test() with the default memory order.
   */
  bool operator[](size_t idx) const;

  /**
   * Return the size of the bitset.
   */
  constexpr size_t size() const { return N; }

 private:
  // Pick the largest lock-free type available
#if (ATOMIC_LLONG_LOCK_FREE == 2)
  using BlockType = unsigned long long;
#elif (ATOMIC_LONG_LOCK_FREE == 2)
  typedef unsigned long BlockType;
#else
  // Even if not lock free, what can we do?
  typedef unsigned int BlockType;
#endif
  using AtomicBlockType = std::atomic<BlockType>;

  static constexpr size_t kBitsPerBlock =
      std::numeric_limits<BlockType>::digits;

  static constexpr size_t blockIndex(size_t bit) { return bit / kBitsPerBlock; }

  static constexpr size_t bitOffset(size_t bit) { return bit % kBitsPerBlock; }

  // avoid casts
  static constexpr BlockType kOne = 1;
  static constexpr size_t kNumBlocks = (N + kBitsPerBlock - 1) / kBitsPerBlock;
  std::array<AtomicBlockType, kNumBlocks> data_;
};

// value-initialize to zero
template <size_t N>
inline ConcurrentBitSet<N>::ConcurrentBitSet() : data_() {}

template <size_t N>
inline bool ConcurrentBitSet<N>::set(size_t idx, std::memory_order order) {
  assert(idx < N);
  BlockType mask = kOne << bitOffset(idx);
  return data_[blockIndex(idx)].fetch_or(mask, order) & mask;
}

template <size_t N>
inline bool ConcurrentBitSet<N>::reset(size_t idx, std::memory_order order) {
  assert(idx < N);
  BlockType mask = kOne << bitOffset(idx);
  return data_[blockIndex(idx)].fetch_and(~mask, order) & mask;
}

template <size_t N>
inline bool ConcurrentBitSet<N>::set(
    size_t idx, bool value, std::memory_order order) {
  return value ? set(idx, order) : reset(idx, order);
}

template <size_t N>
inline bool ConcurrentBitSet<N>::test(
    size_t idx, std::memory_order order) const {
  assert(idx < N);
  BlockType mask = kOne << bitOffset(idx);
  return data_[blockIndex(idx)].load(order) & mask;
}

template <size_t N>
inline bool ConcurrentBitSet<N>::operator[](size_t idx) const {
  return test(idx);
}

} // namespace folly
