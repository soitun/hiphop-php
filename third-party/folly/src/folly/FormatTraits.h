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

#include <cstddef>
#include <type_traits>

namespace folly {
namespace detail {

// Shortcut, so we don't have to use enable_if everywhere
struct FormatTraitsBase {
  using enabled = void;
};

// Traits that define enabled, value_type, and at() for anything
// indexable with integral keys: pointers, arrays, vectors, and maps
// with integral keys
template <class T, class Enable = void>
struct IndexableTraits;

// Base class for sequences (vectors, deques)
template <class C>
struct IndexableTraitsSeq : public FormatTraitsBase {
  using container_type = C;
  using value_type = typename C::value_type;

  static const value_type& at(const C& c, int idx) { return c.at(idx); }

  static const value_type& at(const C& c, int idx, const value_type& dflt) {
    return (idx >= 0 && size_t(idx) < c.size()) ? c.at(idx) : dflt;
  }
};

// Base class for associative types (maps)
template <class C>
struct IndexableTraitsAssoc : public FormatTraitsBase {
  using value_type = typename C::value_type::second_type;

  static const value_type& at(const C& c, int idx) {
    return c.at(static_cast<typename C::key_type>(idx));
  }

  static const value_type& at(const C& c, int idx, const value_type& dflt) {
    auto pos = c.find(static_cast<typename C::key_type>(idx));
    return pos != c.end() ? pos->second : dflt;
  }
};

} // namespace detail
} // namespace folly
