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



#pragma ident "@(#) libf/fio/flsh_mem.c	92.1	06/18/99 16:08:47"

/*
 *	This do-nothing routine's sole purpose in life is to implement the
 *	FLSH_MEM capability on platforms which do not have a #pragma suppress
 *	supported in the C compiler.   A call to this routine accomplishes
 *	the issuing of all prior stores to memory.   The Sparc standard
 *	dictates that stores arrive at memory in the order which they are
 *	issued.  Therefore any stores issued after a call to _flsh_mem()
 *	will arrive in memory after those issued prior to the call to
 *	_flsh_mem().
 */
#include "fio.h"

void
_flsh_mem(void)
{
}
