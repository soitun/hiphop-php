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

#include <limits>
#include <folly/Portability.h>

#include "hphp/util/assertions.h"
#include "hphp/util/portability.h"

extern "C" void* _memcpy8(void* dst, const void* src, size_t len);
extern "C" void* _memcpy16(void* dst, const void* src, size_t len);
extern "C" void _bcopy32(void* dst, const void* src, size_t len);
extern "C" void _bcopy_in_64(void* dst, const void* src, size_t lenIn64);

namespace HPHP {

/*
 * Specialized memcpy implementations that takes advantage of the known
 * properties in length and alignment.
 *
 *  o memcpy8(dst, src, len) is equivalent to
 *        static_cast<char*>(memcpy(dst, src, (len + 7) / 8 * 8)) + len;
 *    it returns a char* pointing to dst[len] instead of dst, in order to
 *    ease its use in string operations.
 *
 *    Note that it could overrun the buffer by up to 7 bytes, depending on len
 *    and alignment of the buffers.  When both src and dst are aligned to 8
 *    bytes, it is safe.  It can also be used in other situations given
 *    sufficient readable space after the buffers.
 *
 *  o memcpy16(dst, src, len) is equivalent to
 *        assert(len > 0 && len % 16 == 0);
 *        memcpy(dst, src, len);
 *
 *  o bcopy32(dst, src, len) is equivalent to
 *         assert(len >= 32);
 *         memcpy(dst, src, len / 32 * 32);
 *    except that it returns void.
 *
 *  o bcopy_in_64(dst, src, lenIn64) is equivalent to
 *         assert(lenIn64 > 0);
 *         memcpy(dst, src, 64 * lenIn64);
 *    except that it returns void.
 */

inline char* memcpy8(void* dst, const void* src, size_t len) {
#if defined(__x86_64__)
  return reinterpret_cast<char*>(_memcpy8(dst, src, len));
#else
  memcpy(dst, src, len);
  return reinterpret_cast<char*>(dst) + len;
#endif
}

inline char* memcpy16(void* dst, const void* src, size_t len) {
  assertx(len > 0 && len % 16 == 0);
#if defined(__x86_64__)
  return reinterpret_cast<char*>(_memcpy16(dst, src, len));
#else
  return reinterpret_cast<char*>(memcpy(dst, src, len));
#endif
}

inline void bcopy32(void* dst, const void* src, size_t len) {
  assertx(len >= 32);
#if defined(__x86_64__)
  _bcopy32(dst, src, len);
#else
  memcpy(dst, src, len / 32 * 32);
#endif
}

inline void bcopy_in_64(void* dst, const void* src, size_t lenIn64) {
  assertx(lenIn64 != 0);
#if defined(__x86_64__)
  _bcopy_in_64(dst, src, lenIn64);
#else
  memcpy(dst, src, lenIn64 * 64);
#endif
}

// Inline assembly version to avoid a function call.
inline void bcopy32_inline(void* dst, const void* src, size_t len) {
  assertx(len >= 32);
#if defined(__x86_64__)
  __asm__ __volatile__("shr    $5, %0\n"
                       ASM_LOCAL_LABEL("BCP32%=") ":\n"
                       "movdqu (%1), %%xmm0\n"
                       "movdqu 16(%1), %%xmm1\n"
                       "add    $32, %1\n"
                       "movdqu %%xmm0, (%2)\n"
                       "movdqu %%xmm1, 16(%2)\n"
                       "add    $32, %2\n"
                       "dec    %0\n"
                       "jg     " ASM_LOCAL_LABEL("BCP32%=") "\n"
                       : "+r"(len), "+r"(src), "+r"(dst)
                       :: "xmm0", "xmm1"
                      );
#elif defined(__aarch64__)
  char __attribute__((vector_size(16))) vt0, vt1;
  __asm__ __volatile__("lsr    %x0, %x0, #5\n"
                       ASM_LOCAL_LABEL("BCP32%=") ":\n"
                       "ldp    %q3, %q4, [%x1], #32\n"
                       "stp    %q3, %q4, [%x2], #32\n"
                       "subs   %x0, %x0, #1\n"
                       "bgt    " ASM_LOCAL_LABEL("BCP32%=") "\n"
                       : "+r"(len), "+r"(src), "+r"(dst),
                         "=&w"(vt0), "=&w"(vt1)
                       :: "cc"
                      );
#else
  bcopy32(dst, src, len);
#endif
}

inline void memcpy16_inline(void* dst, const void* src, size_t len) {
  assertx(len >=16 && len % 16 == 0);
#if defined(__x86_64__)
  __asm__ __volatile__("movdqu -16(%1, %0), %%xmm0\n"
                       "movdqu %%xmm0, -16(%2, %0)\n"
                       "shr    $5, %0\n"
                       "jz     " ASM_LOCAL_LABEL("END%=") "\n"
                       ASM_LOCAL_LABEL("R32%=") ":\n"
                       "movdqu (%1), %%xmm0\n"
                       "movdqu 16(%1), %%xmm1\n"
                       "add    $32, %1\n"
                       "movdqu %%xmm0, (%2)\n"
                       "movdqu %%xmm1, 16(%2)\n"
                       "add    $32, %2\n"
                       "dec    %0\n"
                       "jg     " ASM_LOCAL_LABEL("R32%=") "\n"
                       ASM_LOCAL_LABEL("END%=") ":\n"
                       : "+r"(len), "+r"(src), "+r"(dst)
                       :: "xmm0", "xmm1"
                      );
#elif defined(__aarch64__)
  char __attribute__((vector_size(16))) vt0, vt1;
  int64_t s1, d1;
  __asm__ __volatile__("add    %x5, %x1, %x0\n"
                       "add    %x6, %x2, %x0\n"
                       "ldr    %q3, [%x5, #-16]\n"
                       "str    %q3, [%x6, #-16]\n"
                       "lsr    %x0, %x0, #5\n"
                       "cbz    %x0, " ASM_LOCAL_LABEL("END%=") "\n"
                       ASM_LOCAL_LABEL("R32%=") ":\n"
                       "ldp    %q3, %q4, [%x1], #32\n"
                       "stp    %q3, %q4, [%x2], #32\n"
                       "subs   %x0, %x0, #1\n"
                       "bgt    " ASM_LOCAL_LABEL("R32%=") "\n"
                       ASM_LOCAL_LABEL("END%=") ":\n"
                       : "+r"(len), "+r"(src), "+r"(dst),
                         "=&w"(vt0), "=&w"(vt1),
                         "=&r"(s1), "=&r"(d1)
                       :: "cc"
                      );
#else
  memcpy16(dst, src, len);
#endif
}

//////////////////////////////////////////////////////////////////////

/*
 * Word at a time comparison for two strings of length `lenBytes'.  Returns
 * true if the regions are the same. This should be invoked only when we know
 * the two strings have the same length. It will not check for the null
 * terminator.
 *
 * Assumes that the buffer addresses are word aligned, and that it can read
 * lenBytes rounded up to a whole word. This is possible in HPHP because we
 * always allocate whole numbers of words.  The final word compare is adjusted
 * to handle the slack in lenBytes so only the bytes we care about are
 * compared.
 */
ALWAYS_INLINE
bool wordsame(const void* mem1, const void* mem2, uint32_t lenBytes) {
  using T = uint64_t;
  auto constexpr DEBUG_ONLY W = sizeof(T);

  assert(reinterpret_cast<const uintptr_t>(mem1) % W == 0);
  assert(reinterpret_cast<const uintptr_t>(mem2) % W == 0);

// ASan is less precise than valgrind and believes this function overruns reads
#if !FOLLY_SANITIZE

  // For speed, we count up towards 0 from -lenBytes * 8 in units of a word of
  // bits. When we reach a value >= 0, that is the number of bits we need to
  // ignore in the last compare. Since we're on a little-endian architecture,
  // we can do the ignoring by shifting left by that many bits. We also unroll
  // the nBits increment from the first iteration, because we can fold that
  // calculation together with the multiply by 8 into a single lea instruction.
  const int32_t nBytes = -lenBytes;
  // We need to bail out early if len is 0, and we can save a test instruction
  // if we reuse the flags from the negation we just did.
  if (UNLIKELY(nBytes == 0)) return true;
  int64_t nBits = int64_t(nBytes) * 8 + (W * 8);

  // Use the base+index addressing mode in x86, so that we only need to
  // increment the base pointer in the loop.
  auto p1 = reinterpret_cast<intptr_t>(mem1);
  auto const diff = reinterpret_cast<intptr_t>(mem2) - p1;

  T data;
  do {
    data = *(reinterpret_cast<const T*>(p1));
    data ^= *(reinterpret_cast<const T*>(p1 + diff));
    if (nBits >= 0) {
      // As a note for future consideration, we could consider precomputing a
      // 64-bit mask, so that the fraction of the last qword can be checked
      // faster.  But that would require an additional register for the
      // mask.  So it depends on register pressure of the call site.
      return !(data << nBits);
    }
    p1 += W;
    nBits += W * 8;
  } while (data == 0);
  return false;

#else // FOLLY_SANITIZE

  return !memcmp(mem1, mem2, lenBytes);

#endif
}

/*
 * Like memcpy, but copies numT POD values 8 bytes at a time.
 * The actual number of bytes copied must be a multiple of 8.
 */
template<class T>
T* wordcpy(T* to, const T* from, size_t numT) {
  assert(numT < std::numeric_limits<int64_t>::max() &&
         (numT * sizeof(T)) % 8 == 0);
  size_t numWords = numT * sizeof(T) / 8;
  assert(numWords != 0);
  auto d = (int64_t*)to;
  auto s = (int64_t*)from;
  do {
    *d++ = *s++;
  } while (--numWords);
  return to;
}

/*
 * Fills a memory area with ones, 8 bytes at a time.
 */
template<class T>
T* wordfillones(T* ptr, size_t numT) {
  assert(numT < std::numeric_limits<int64_t>::max() &&
         (numT * sizeof(T)) % 8 == 0);
  assert(numT != 0);
  auto numWords = numT * sizeof(T) / 8;
  auto d = (int64_t*)ptr;
  do {
    *d++ = -1;
  } while (--numWords);
  return ptr;
}

//////////////////////////////////////////////////////////////////////

}
