/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#pragma ident "@(#) libf/pxf/ipxfargc.c	92.1	06/29/99 11:36:06"

#include <fortran.h>

#ifdef _UNICOS

extern	int	_argc;		/* Number of argv fields */

_f_int
IPXFARGC()
{
	return((_f_int) (_argc - 1) );
}

#elif defined(__mips)

extern  int     __Argc;         /* Number of argv fields */
#pragma weak    __Argc

_f_int
ipxfargc_()
{
	return((_f_int) (__Argc - 1) );
}
#elif defined(__linux)
extern  int     f__xargc;            /* Number of argv fields */

_f_int
ipxfargc_()
{
	return((_f_int) (f__xargc - 1) );
}

#elif defined(BUILD_OS_DARWIN)

extern  int     NXArgc;         /* Number of argv fields */

_f_int
ipxfargc_()
{
        return (_f_int) (NXArgc - 1);
}

#else

extern  int     __xargc;         /* Number of argv fields */

_f_int
ipxfargc_()
{
        return((_f_int) (__xargc - 1) );
}

#endif
