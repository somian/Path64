/*
 * Copyright (C) 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
