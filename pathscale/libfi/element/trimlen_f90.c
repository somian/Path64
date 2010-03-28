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


#pragma ident "@(#) libfi/element/trimlen_f90.c	92.3	06/16/99 15:47:23"

#include <fortran.h>
extern _f_int trimlen_(char *carg, _f_int len_carg);

_f_int
trimlen_(char *carg, _f_int len_carg)
{
	_f_int i;
	if (len_carg == 0)
		return(0);
	for (i = len_carg - 1; i >= 0; i--) {
		if (carg[i] != ' ')
			return (i+1);
	}
	return(1);
}
