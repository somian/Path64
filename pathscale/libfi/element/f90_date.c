/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#pragma ident "@(#) libfi/element/f90_date.c	92.1	06/16/99 15:47:23"

#include <fortran.h>

#ifdef KEY /* Bug 8290 */
#  include "element.h"
#else /* KEY Bug 8290 */
extern void _DATE_( void *dayofyear, int  iffcd );
#endif /* KEY Bug 8290 */

/*	
 *      Duplicate names
 *
 *      _F90_DATE_	- for f90 intrinsic on non-UNICOS systems
 *      date_		- when called as a subroutine on non-UNICOS systems
 */

void
_F90_DATE_( char *ptr, int len )
{
    _DATE_(ptr, len);
}

void date_( char *ptr, int len)		{ _F90_DATE_(ptr, len); }
