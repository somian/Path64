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


/* $Header: /home/bos/bk/kpro64-pending/libF77/s_srng.c 1.5 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <stdio.h>
#include "cmplrs/host.h"


/* called when a substring lower bound is out of range */

void
s_srng(char *varn, int32 lower, int32 upper, char *procn, int32 line)
{
	register int32 i;
	extern void s_abort(int32);

	fprintf(stderr, "Substring bound(s) out of range on file line %d, procedure ", line);
	for(i = 0 ; i < 8 && *procn!='_' ; ++i)
		putc(*procn++, stderr);
	fprintf(stderr, ".\nAttempt to access the %d-th through the %d-th characters of variable ", lower, upper);
	for(i = 0 ; i < 6  && *varn!=' ' ; ++i)
		putc(*varn++, stderr);
	fprintf(stderr, ".\n");
	s_abort(99);
}
