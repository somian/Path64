/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libfi/mathlb/sign.c	92.1	09/29/99 18:35:37"

#include <fortran.h>
#include <math.h>

/*
 * SIGN: real(kind=8) - pass by address
 *    Return the absolute value of x with the sign of y.
 */

/*
 * On mips, only 32-bit float and 64-bit double sign can be passed as
 * arguments and thus have addresses of values rather than values for
 * arguments.  (SIGN and DSIGN)
 *
 * When f90 specifies DSIGN for default 32 or SIGN for -default64,
 * the two entry points are:
 *
 *   __d_sign_ when f90 -LANG:IEEE_minus_zero is present.
 *   d_sign when f90 -LANG:IEEE_minus_zero is not present.
 *
 * This routine is necessary to get the routine to recognize -0.0
 */
union _dflt {
	_f_real8	dbl;
	struct {
#if defined(_LITTLE_ENDIAN)
		unsigned int flt2	: 32;
		unsigned int flt1	: 31;
		unsigned int sign	: 1;
#else
		unsigned int sign	: 1;
		unsigned int flt1	: 31;
		unsigned int flt2	: 32;
#endif 
	} parts;
};

#if     defined(__mips) || defined(_LITTLE_ENDIAN)
extern _f_real8 __d_sign_(_f_real8 *x,  _f_real8 *y);
_f_real8
__d_sign_( _f_real8 *x,
        _f_real8 *y)
{
	union _dflt	f, rslt;
	rslt.dbl	= *x;
	f.dbl		= *y;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbl);
}

extern _f_real8 __d_sign(_f_real8 *x,  _f_real8 *y);
_f_real8
__d_sign( _f_real8 *x,
        _f_real8 *y)
{
	union _dflt	f, rslt;
	rslt.dbl	= *x;
	f.dbl		= *y;
	/* If the second argument is -0.0, reset the sign. */
	if (f.dbl == 0)
		f.parts.sign	= 0;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbl);
}
#else
extern _f_real8 _SIGN_(_f_real8 *x,  _f_real8 *y);
_f_real8
_SIGN_( _f_real8 *x,
	_f_real8 *y)
{
	union _dflt	f, rslt;
	rslt.dbl	= *x;
	f.dbl		= *y;
	rslt.parts.sign	= f.parts.sign;
	return (rslt.dbl);
}
#endif

