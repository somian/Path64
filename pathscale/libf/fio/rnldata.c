/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/rnldata.c	92.2	06/21/99 10:37:55"
 
#include "fio.h"

/* This module contains data used by the namelist read routines */

long	_SKP_MESS	= 1;		/* If nonzero give a warning message 
					 * for skipped namelist groups on
					 * input. */
unum_t	_OUT_UNIT	= -1;		/* Unit where echoed input lines are
					 * written.  -1 indicates default */
long	_TYP_CONV	= 1;		/* If nonzero, type conversions are
					 * allowed across the equal sign. */
long	_BLNKSEP	= 1;		/* 0 if user has turned off the use 
					 * of a space character as a delimiter*/

/*
 * The _MASKS[] array contains bit flags for use in determining type.
 * Each pair of 64-bit words is a bit mask for the 128 ASCII characters.
 * The corresponding bit is set if the character belongs to a particular
 * group.
 */

long long
_MASKS[]	= {
#ifdef	_UNICOS
	0000000000001200000000,		/* $ & namelist delimiters */
	0,

	0000000000000000177700,		/* 0..9 A..Z a..z */
	0777777776017777777740, 

	0000000000000000000004,		/* = */
	0,

	0000000000000002000000,		/* , separator	*/
	0,

	0,				/* E e	echo indicator */
	0020000000000400000000,

	0000000000010000000060,		/* : ; ! comment indicator */
	0
};
#elif	defined(_SOLARIS) || defined(__mips) || defined(_ABSOFT) || \
	defined(_LITTLE_ENDIAN)
	0000000000001200000000LL,	/* $ & namelist delimiters */
	0LL,

	0000000000000000177700LL,	/* 0..9 A..Z a..z */
	0777777776017777777740LL, 

	0000000000000000000004LL,	/* = */
	0LL,

	0000000000000002000000LL,	/* , separator	*/
	0LL,

	0LL,				/* E e	echo indicator */
	0020000000000400000000LL,

	0000000000010000000060LL,	/* : ; ! comment indicator */
	0LL
};
#endif  /* UNICOS */
