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



#pragma ident "@(#) libu/numconv/mpp/sd2uese.c	92.1	06/28/99 15:00:40"

#include <cray/fmtconv.h>

/*
	General:
		Convert the value to the formatted ASCII character field 
		specified.  Using the Fortran "ESw.dEe" format.
		The "ESw.dEe" descriptor is the same as the "1pEw.dEe"
		descriptor, so a call is made to the _sd2udee routine
		with the scale factor p = 1.

	Inputs:
		value - value to output
		fca - first character address of ASCII output field
		mode - flags to determine sign behavior
			0 = only minus sign required
			1 = sign required
		w - total width of character field
		d - width of field after decimal point
		e - field width of exponent
		p - ignored

	Outputs:
		value - unchanged
		fca - unchanged
		mode - unchanged
		w - unchanged
		d - unchanged
		e - unchanged
		p - unchanged

        Examples:
		With value=0.105e10, mode=1, w=10, d=2, e=0 we will end up
		with a field filled with " +1.05E+09"

		With value=-9.995e-10, mode=0, w=10, d=2, e=2 we will end
		up with a field filled with " -1.00E-09"

		With value=-0.005e-5, mode=0, w=12, d=2, e=4 we will end up
		with a field filled with " -5.00E-0008"

		With value=0.55, mode=0, w=8, d=2, e=0 we will end up
		with a field filled with "5.50E-01"

*/
long
*_sd2uese(	const void	*value,
		long		*fca,
		const long	*mode,
		const long	*w,
		const long	*d,
		const long	*e,
		const long	*p)
{
	long tmp = 1;
	return(_sd2udee(value,fca,mode,w,d,e,&tmp));
}
