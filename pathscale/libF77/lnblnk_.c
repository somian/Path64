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


/* $Header: /home/bos/bk/kpro64-pending/libF77/lnblnk_.c 1.5 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

/*	function lnblnk(s)
 *		Returns index of last nonblank character in string s
 */

#include <cmplrs/host.h>
#include <string.h>

#ifdef KEY /* Bug 1683 */

/* Don't pollute the Fortran namespace */
int32
pathf90_lnblnk(string s, fsize_t len)

#else

int32
lnblnk_ (string s, fsize_t len)

#endif /* KEY Bug 1683 */
{	
    register string e;
    
    e=s+len-1;
    while  (e>=s && *e == ' ') e--;
    return(e-s+1);
}
