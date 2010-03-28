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


#pragma ident "@(#) libfi/mathlb/ieee_unordered.c	92.1	07/09/99 11:00:36"

#include <fortran.h>
#include "inline.h"

#if _F_REAL16 == 1
extern _f_log4 _IEEE_UNORDERED_L4_D_D( _f_real16 x, _f_real16 y);

/* Function returns TRUE if either 128-bit real argument is a NaN.
 * Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_D_D( _f_real16 x, _f_real16 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan128(x) || isnan128(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_D_D( _f_real16 x, _f_real16 y);

_f_log8 _IEEE_UNORDERED_L8_D_D( _f_real16 x, _f_real16 y)
{
	/* if x is NaN , return TRUE */
	return ((_f_log8) _btol(isnan128(x) || isnan128(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_D_R( _f_real16 x, _f_real8 y);

/* Function returns TRUE if one of the 128-bit or 64-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_D_R( _f_real16 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan128(x) || isnan64(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_D_R( _f_real16 x, _f_real8 y);

_f_log8 _IEEE_UNORDERED_L8_D_R( _f_real16 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan128(x) || isnan64(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_D_H( _f_real16 x, _f_real4 y);

/* Function returns TRUE if one of the 128-bit or 32-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_D_H( _f_real16 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan128(x) || isnan32(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_D_H( _f_real16 x, _f_real4 y);

_f_log8 _IEEE_UNORDERED_L8_D_H( _f_real16 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan128(x) || isnan32(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_R_D( _f_real8 x, _f_real16 y);

/* Function returns TRUE if one of the 64-bit or 128-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_R_D( _f_real8 x, _f_real16 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan64(x) || isnan128(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_R_D( _f_real8 x, _f_real16 y);

_f_log8 _IEEE_UNORDERED_L8_R_D( _f_real8 x, _f_real16 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan64(x) || isnan128(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_H_D( _f_real4 x, _f_real16 y);

/* Function returns TRUE if one of the 32-bit or 128-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_H_D( _f_real4 x, _f_real16 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan32(x) || isnan128(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_H_D( _f_real4 x, _f_real16 y);

_f_log8 _IEEE_UNORDERED_L8_H_D( _f_real4 x, _f_real16 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan32(x) || isnan128(y)));
}

#endif  /* _F_REAL16 */


extern _f_log4 _IEEE_UNORDERED_L4_R_R( _f_real8 x, _f_real8 y);

/* Function returns TRUE if one of two 64-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_R_R( _f_real8 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan64(x) || isnan64(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_R_R( _f_real8 x, _f_real8 y);

_f_log8 _IEEE_UNORDERED_L8_R_R( _f_real8 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan64(x) || isnan64(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_R_H( _f_real8 x, _f_real4 y);

/* Function returns TRUE if one of the 64-bit or 32-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_R_H( _f_real8 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan64(x) || isnan32(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_R_H( _f_real8 x, _f_real4 y);

_f_log8 _IEEE_UNORDERED_L8_R_H( _f_real8 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan64(x) || isnan32(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_H_R( _f_real4 x, _f_real8 y);

/* Function returns TRUE if one of the 32-bit or 64-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_H_R( _f_real4 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan32(x) || isnan64(y)));
}


extern _f_log8 _IEEE_UNORDERED_L8_H_R( _f_real4 x, _f_real8 y);

_f_log8 _IEEE_UNORDERED_L8_H_R( _f_real4 x, _f_real8 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan32(x) || isnan64(y)));
}


extern _f_log4 _IEEE_UNORDERED_L4_H_H( _f_real4 x, _f_real4 y);

/* Function returns TRUE if one of two 32-bit real arguments
 * is a NaN.  Else, it returns FALSE.
 */
_f_log4 _IEEE_UNORDERED_L4_H_H( _f_real4 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log4) _btol(isnan32(x) || isnan32(y)));
}

extern _f_log8 _IEEE_UNORDERED_L8_H_H( _f_real4 x, _f_real4 y);

_f_log8 _IEEE_UNORDERED_L8_H_H( _f_real4 x, _f_real4 y)
{
	/* if x is NaN, return TRUE */
	return ((_f_log8) _btol(isnan32(x) || isnan32(y)));
}
