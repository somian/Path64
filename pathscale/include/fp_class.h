/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#ifndef __FP_CLASS_H__
#define __FP_CLASS_H__

#ifndef __GNUC__
#ident "$Revision$"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* | All Rights Reserved.                            | */
/* --------------------------------------------------- */
/* $Revision$ */



#if ((defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)) && !defined(__GNUC__))
extern int fp_class_d(double);
extern int fp_class_f(float);
#if _COMPILER_VERSION >= 400
extern int fp_class_l(long double);
#define	fp_class_q fp_class_l
#endif
#endif /* LANGUAGE_C */

/*
 * Constants returned by the floating point fp_class_[fdq]() functions.
 */
#if defined(BUILD_OS_DARWIN)
/* Assuming we're using our own fp_class_[fdq]() functions and not the
 * standard fpclassify, we want to ignore any definitions from the environ
 * outside the compiler */
#undef FP_SNAN
#undef FP_QNAN
#endif /* defined(BUILD_OS_DARWIN) */
#define	FP_SNAN		0
#define	FP_QNAN		1
#define	FP_POS_INF	2
#define	FP_NEG_INF	3
#define	FP_POS_NORM	4
#define	FP_NEG_NORM	5
#define	FP_POS_DENORM	6
#define	FP_NEG_DENORM	7
#define	FP_POS_ZERO	8
#define	FP_NEG_ZERO	9

#ifdef __cplusplus
}
#endif
#endif /* !__FP_CLASS_H__ */
