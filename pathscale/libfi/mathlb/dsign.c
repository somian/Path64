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


#pragma ident "@(#) libfi/mathlb/dsign.c	92.1	09/29/99 18:35:37"

#include <fortran.h>
#include <math.h>

/*
 * DSIGN: SIGN of real(kind=16)  - pass by address
 * 128-bit float sign
 *    Return the absolute value of x with the sign of y.
 */

/*
 * On mips, only 32-bit float and 64-bit double sign can be passed as
 * arguments and thus have addresses of values rather than values for
 * arguments.  (SIGN and DSIGN)
 *
 * When f90 -default64 is used, the two entry points for DSIGN are:
 *  
 *   __q_sign_ when f90 -LANG:IEEE_minus_zero is present.
 *   __q_sign when f90 -LANG:IEEE_minus_zero is not present.
 *
 * This routine is necessary to get the rout8ine to recognize -0.0
 */

union _ldflt {
	_f_real16	dbldbl;
	struct {
		unsigned int sign	: 1;
		unsigned int flt1	: 31;
		unsigned int flt2	: 32;
		unsigned int flt3	: 32;
		unsigned int flt4	: 32;
	} parts;
};

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
extern _f_real16  __q_sign_(_f_real16 *x, _f_real16 *y);
_f_real16
__q_sign_( _f_real16 *x,
	_f_real16 *y)
{
	union _ldflt	f, rslt;
	rslt.dbldbl	= *x;
	f.dbldbl	= *y;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbldbl);
}

extern _f_real16  __q_sign(_f_real16 *x, _f_real16 *y);
_f_real16
__q_sign( _f_real16 *x,
	_f_real16 *y)
{
	union _ldflt	f, rslt;
	rslt.dbldbl	= *x;
	f.dbldbl	= *y;
	/* If the second argument is -0.0, reset the sign. */
	if (f.dbldbl == 0)
		f.parts.sign	= 0;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbldbl);
}
#else
extern _f_real16  _DSIGN_(_f_real16 *x, _f_real16 *y);
_f_real16
_DSIGN_( _f_real16 *x,
	_f_real16 *y)
{
	union _ldflt	f, rslt;
	rslt.dbldbl	= *x;
	f.dbldbl	= *y;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbldbl);
}
#endif
