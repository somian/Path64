/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


/* USMID @(#) libfi/include/arraydefs.h	92.0	10/08/98 14:37:14 */

#if defined(_F_LOG4) && defined(_F_LOG8)
#include <limits.h>
#endif

#define DVSUBTYPE_BIT32		9
#define	DVSUBTYPE_BIT64		10
#define DVSUBTYPE_BIT128	11
#define	DVSUBTYPE_BIT256	12
#define	DVSUBTYPE_CHAR		13
#define	DVSUBTYPE_DERIVED	14
#define DVSUBTYPE_INT32		15
#ifdef _F_INT6
#define DVSUBTYPE_INT46		16
#endif
#define DVSUBTYPE_INT64		17
#define DVSUBTYPE_REAL64	18
#define DVSUBTYPE_REAL128	19
#define DVSUBTYPE_REAL32	20
#ifdef _F_COMP4
#define	DVSUBTYPE_COMP64	21
#endif
#define DVSUBTYPE_BIT16		22
#define DVSUBTYPE_BIT8		23

#ifdef _F_COMP16
typedef struct {
	_f_real16 re;	/* real part		*/
	_f_real16 im;	/* imaginary part	*/
	} dblcmplx;
#endif

typedef _f_dble	dblword;	/* _f_dble defined in fortran.h	*/

#if	defined(_F_LOG2) && defined(__mips)
#define LTOB(i,a) \
	((i == 8*CHAR_BIT) ? (_lvtob((*(_f_log8 *) (a)))) : \
	((i == 4*CHAR_BIT) ? (_lvtob((*(_f_log4 *) (a)))) : \
	((i == 2*CHAR_BIT) ? (_lvtob((*(_f_log2 *) (a)))) : \
	((i == 1*CHAR_BIT) ? (_lvtob((*(_f_log1 *) (a)))) : \
	(_lvtob((*(_f_log *) (a))))))))
#elif defined(_F_LOG4) && defined(_F_LOG8)
#define LTOB(i,a) \
 ((i == 4*CHAR_BIT) ? (_lvtob((*(_f_log4 *) (a)))) : (_lvtob((*(_f_log8 *) (a)))))
#else
#define LTOB(i,a) (_lvtob((*(_f_log *) (a))))
#endif

#ifdef _F_REAL4
#define HUGE_S4_REAL	HUGE_REAL4_F90
#endif
#define HUGE_S8_REAL	HUGE_REAL8_F90
#define HUGE_S16_REAL	HUGE_REAL16_F90

#define BITS_PER_BYTE	(BITS_PER_WORD / BYTES_PER_WORD)

#ifdef	_F_LOG4
typedef	_f_log4		_f_mask;
#else
typedef	_f_log		_f_mask;
#endif
