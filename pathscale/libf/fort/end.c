/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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



#pragma ident "@(#) libf/fort/end.c	92.1	06/24/99 10:18:36"
  
#include <stdlib.h>
#include "fio.h"

/*
 *	_END	Process Fortran END statement
 */
#ifdef _CRAY
#pragma _CRI duplicate _END as $END
#endif
void
_END(void)
{
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	/* We need to call _fcleanup before */
	/* we get into exit processing. Sometimes a code that uses */
	/* asynchronous i/o does not go through its atexit() routine. */
	_fcleanup();
#endif
	exit(0);  
}
