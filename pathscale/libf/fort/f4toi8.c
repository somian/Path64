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



#pragma ident "@(#) libf/fort/f4toi8.c	92.1	06/24/99 10:18:36"

#include <fortran.h>

extern _f_int8 _F4TOI8(_f_real4 arg);

/* Convert 32-bit real to 64-bit integer for the cf90_s compiler only.
 * The argument is passed by value. Use a cast for the current time.
 */

_f_int8
_F4TOI8(_f_real4 arg)
{
	return ( (_f_int8) arg);
}
