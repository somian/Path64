#pragma ident "@(#)92/math/floorl.c	92.1	06/02/99 16:43:34"

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


/* floorl is floor for long double arguments. */

#if __STDC__

#include "synonyms.h"
#include <math.h>
#include <fp.h>
#include <errno.h>

extern long double DINT();

long double
floorl(x)
long double x;
{
	long double l;
	long double f;

#if defined(_CRAYIEEE)
	if (isnan(x)) {
		errno = EDOM;
		return(__NANL);
	}
#endif
	/* Determine the fraction part and subtract it from original value. */
	l = DINT(&x);
	f = x - l;
	x = x - f;
	return ( (f < 0) ? x - 1.0 : x );
}

#endif
