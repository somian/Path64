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


#pragma ident "@(#) libfi/mathlb/ieee_class_r.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include "ieee_class_values.h"
#include "inline.h"

extern _f_int4 _IEEE_CLASS_I4( _f_real8 x);

/* _IEEE_CLASS_D returns a value indicating the class of the argument.
 * The classes are listed in MODULE CRI_IEEE_DEFINITIONS.
 * Argument:  64-bit real value
 * Result:    32-bit integer
 */
_f_int4 _IEEE_CLASS_I4( _f_real8 x)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_doble {
		_f_real8	dword;
		struct {
#if defined(_LITTLE_ENDIAN)
			unsigned int mantissa_lo : IEEE_64_MANT_BTS2;
			unsigned int mantissa_up : IEEE_64_MANT_BTS1-1;
			unsigned int q_bit  : 1;
			unsigned int exponent : IEEE_64_EXPO_BITS;
			unsigned int sign     : 1;
#else
			unsigned int sign     : 1;
			unsigned int exponent : IEEE_64_EXPO_BITS;
			unsigned int q_bit  : 1;
			unsigned int mantissa_up : IEEE_64_MANT_BTS1-1;
			unsigned int mantissa_lo : IEEE_64_MANT_BTS2;
#endif
		} parts;
	};
	union	_ieee_doble x_val;

	x_val.dword		= x;
	if (x_val.parts.exponent == 0) {
		if (x_val.parts.mantissa_up == 0 &&
		    x_val.parts.mantissa_lo == 0 &&
		    x_val.parts.q_bit == 0) {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_ZERO);
			else
				return (IEEE_CLASS_POS_ZERO);
		} else {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_DENORM);
			else
				return (IEEE_CLASS_POS_DENORM);
		}
	} else if (x_val.parts.exponent == IEEE_64_EXPO_MAX) {
		if (x_val.parts.mantissa_up == 0 &&
		    x_val.parts.mantissa_lo == 0 &&
		    x_val.parts.q_bit == 0) {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_INFINITY);
			else
				return (IEEE_CLASS_POS_INFINITY);
		} else {
			if (x_val.parts.q_bit)
				return (IEEE_CLASS_QUIET_NAN);
			else
				return (IEEE_CLASS_SIGNALING_NAN);
		}
	} else if (x_val.parts.sign)
		return (IEEE_CLASS_NEG_NORM_NONZERO);
	else
		return (IEEE_CLASS_POS_NORM_NONZERO);
}


extern _f_int8 _IEEE_CLASS_I8( _f_real8 x);

/* Argument:  64-bit real value
 * Result:    64-bit integer
 */
_f_int8 _IEEE_CLASS_I8( _f_real8 x)
{
	/* Union defined to work with IEEE 64-bit floating point. */
	union _ieee_doble {
		_f_real8	dword;
		struct {
#if defined(_LITTLE_ENDIAN)
			unsigned int mantissa_lo : IEEE_64_MANT_BTS2;
			unsigned int mantissa_up : IEEE_64_MANT_BTS1-1;
			unsigned int q_bit  : 1;
			unsigned int exponent : IEEE_64_EXPO_BITS;
			unsigned int sign     : 1;
#else
			unsigned int sign     : 1;
			unsigned int exponent : IEEE_64_EXPO_BITS;
			unsigned int q_bit  : 1;
			unsigned int mantissa_up : IEEE_64_MANT_BTS1-1;
			unsigned int mantissa_lo : IEEE_64_MANT_BTS2;
#endif
		} parts;
	};
	union	_ieee_doble x_val;

	x_val.dword		= x;
	if (x_val.parts.exponent == 0) {
		if (x_val.parts.mantissa_up == 0 &&
		    x_val.parts.mantissa_lo == 0 &&
		    x_val.parts.q_bit == 0) {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_ZERO);
			else
				return (IEEE_CLASS_POS_ZERO);
		} else {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_DENORM);
			else
				return (IEEE_CLASS_POS_DENORM);
		}
	} else if (x_val.parts.exponent == IEEE_64_EXPO_MAX) {
		if (x_val.parts.mantissa_up == 0 &&
		    x_val.parts.mantissa_lo == 0 &&
		    x_val.parts.q_bit == 0) {
			if (x_val.parts.sign)
				return (IEEE_CLASS_NEG_INFINITY);
			else
				return (IEEE_CLASS_POS_INFINITY);
		} else {
			if (x_val.parts.q_bit)
				return (IEEE_CLASS_QUIET_NAN);
			else
				return (IEEE_CLASS_SIGNALING_NAN);
		}
	} else if (x_val.parts.sign)
		return (IEEE_CLASS_NEG_NORM_NONZERO);
	else
		return (IEEE_CLASS_POS_NORM_NONZERO);
}
