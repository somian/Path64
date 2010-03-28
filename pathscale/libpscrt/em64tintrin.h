/*
 * Copyright 2007 PathScale, LLC.  All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

#ifndef __em64tintrin_h
#define __em64tintrin_h

#if __GNUC__ <= 3
typedef int __v2di __attribute__ ((mode (V2DI)));
#else
typedef long long __v2di __attribute__ ((__vector_size__ (16)));
#endif

typedef __v2di __m128i;

enum _mm_hint
{
  _MM_HINT_T0 = 3,
  _MM_HINT_T1 = 2,
  _MM_HINT_T2 = 1,
  _MM_HINT_NTA = 0
};

#define _mm_prefetch(P, I) \
  __builtin_prefetch ((P), 0, (I))

static __inline __m128i __attribute__((__always_inline__))
_mm_load_si128 (__m128i const *__P)
{
  return *__P;
}

static __inline void __attribute__((__always_inline__))
_mm_store_si128 (__m128i *__P, __m128i __B)
{
  *__P = __B;
}

static __inline void __attribute__((__always_inline__))
_mm_stream_si128 (__m128i *__A, __m128i __B)
{
  __builtin_ia32_movntdq ((__v2di *)__A, (__v2di)__B);
}

static __inline __m128i __attribute__((__always_inline__))
_mm_loadu_si128 (__m128i const *__P)
{
  return (__m128i) __builtin_ia32_loaddqu ((char const *)__P);
}

static __inline void __attribute__((__always_inline__))
_mm_sfence (void)
{
  __builtin_ia32_sfence ();
}

#endif /* __em64tintrin_h */
