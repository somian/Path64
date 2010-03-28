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


/* $Header: /home/bos/bk/kpro64-pending/libF77/subout.c 1.6 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <stdio.h>
#include <stdlib.h>
#include "cmplrs/host.h"

void
subout(char *varn, int32 offset, char *procn, int32 line)
{
	register int32 i;
	extern void _cleanup(void);

	fprintf(stderr, "Subscript out of range on line %d of procedure ", line);
	for(i = 0 ; i < 8 && *procn!='_' ; ++i)
		putc(*procn++, stderr);
	fprintf(stderr, ".\nAttempt to access the %d-th element of variable ", offset+1);
	for(i = 0 ; i < 6  && *varn!=' ' ; ++i)
		putc(*varn++, stderr);
	fprintf(stderr, ".\n");
	fflush(stdout);
	fflush(stderr);
	abort();
}
