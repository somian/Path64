/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 *
 * Unpublished -- rights reserved under the copyright laws of the
 * United States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
 * TRADE SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION
 * IS PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF
 * PATHSCALE, INC.
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
 * PathScale, Inc.
 * 2071 Stierlin Court, Suite 200
 * Mountain View CA 94043
 * USA
 */

#ifndef __intrin_h
#define __intrin_h

#if (defined __GNUC__) &&			\
    (defined __GNUC_MINOR__) &&			\
    defined (__GNUC_PATCHLEVEL__)
#define GNUC_VERSION ((__GNUC__ * 100) + 	\
                      (__GNUC_MINOR__ * 10) +	\
                      __GNUC_PATCHLEVEL__)
#else
#define GNUC_VERSION 0
#endif

/* NOTE: have not looked at the intrinsics headers for gcc < 3.2.2 */

#if GNUC_VERSION >= 322
#define MMINTRIN_H_AVAILABLE
#define XMMINTRIN_H_AVAILABLE
#endif

#if GNUC_VERSION >= 323
#define PMMINTRIN_H_AVAILABLE
#endif

#if GNUC_VERSION >= 332
#define EMMINTRIN_H_AVAILABLE
#endif

#ifdef XMMINTRIN_H_AVAILABLE
#include <xmmintrin.h>
#else
#error Cannot compile intrinsics without <xmmintrin.h>
#endif

#ifdef EMMINTRIN_H_AVAILABLE
#include <emmintrin.h>
#elif defined __m128i
/* NOTE: gcc 3.3.1 on SuSE Linux 9.0 provides SSE2 intrinsics in xmmintrin.h */
#else
/* Map required SSE2 intrinsics onto closest SSE intrinsics */
#define __m128i __m128
#define _mm_load_si128(a) _mm_load_ps((float *) (a))
#define _mm_loadu_si128(a) _mm_loadu_ps((float *) (a))
#define _mm_store_si128(a,b) _mm_store_ps((float *) (a), b)
#define _mm_stream_si128(a,b) _mm_stream_ps((float *) (a), b)
#endif

#endif
