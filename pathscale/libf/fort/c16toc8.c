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



#pragma ident "@(#) libf/fort/c16toc8.c	92.1	06/24/99 10:18:36"

#include <fortran.h>

extern void _C16TOC8(_f_comp8 *ret_val, _f_comp16 arg);

/* Convert 256-bit complex to 128-bit complex for the cf90_s compiler
 * only. The argument is a structure and the result is an address of a
 * structure.
 */

void
_C16TOC8(_f_comp8 *ret_val,
	_f_comp16 arg)
{
	ret_val->x[0] = (_f_real8) arg.x[0];
	ret_val->x[1] = (_f_real8) arg.x[1];
	return;
}
