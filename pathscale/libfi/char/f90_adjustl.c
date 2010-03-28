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


#pragma ident "@(#) libfi/char/f90_adjustl.c	92.1	07/08/99 10:41:51"

#include <fortran.h>

extern void _ADJUSTL_( _fcd result, _fcd str );
		   
/*
 *	Note that _F90_ADJUSTL is not a "vanilla" calling sequence for
 *	a Sparc routine which returns a character value.   
 */
void 
_F90_ADJUSTL(
char *res_addr,
char *src_addr,
int  res_len,
int  src_len )
{
    _ADJUSTL_(_cptofcd(res_addr, res_len), _cptofcd(src_addr, src_len));
}
