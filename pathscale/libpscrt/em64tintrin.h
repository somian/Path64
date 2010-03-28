/*
 * Copyright 2007 PathScale, LLC.  All Rights Reserved.
 *
 * Unpublished -- rights reserved under the copyright laws of the
 * United States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
 * TRADE SECRETS OF PATHSCALE, LLC. USE, DISCLOSURE, OR REPRODUCTION
 * IS PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF
 * PATHSCALE, LLC.
 *
 * U.S. Government Restricted Rights:
 * The Software is a "commercial item," as that term is defined at 48
 * C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
 * and "commercial computer software documentation," as such terms are used
 * in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
 * 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
 * End Users acquire the Software with only those rights set forth in the
 * accompanying license agreement.
 *
 * PathScale, LLC
 * 2071 Stierlin Court, Suite 200
 * Mountain View CA 94043
 * USA
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
