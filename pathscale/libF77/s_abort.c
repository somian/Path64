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


/* $Header: /home/bos/bk/kpro64-pending/libF77/s_abort.c 1.5 04/12/21 14:58:04-08:00 bos@eng-25.internal.keyresearch.com $ */

/* called when a fatal error wishes to terminate execution */

#include <stdio.h>
#include "cmplrs/host.h"
#include <stdlib.h>

extern int up_low(char);

void
s_abort(int32 errno)
{
char *dumpflag, *getenv();
int32 coredump=0;
#ifndef FTN90_IO
void _cleanup();
#endif

	if (dumpflag = getenv("f77_dump_flag")) {
		coredump = up_low(*dumpflag) == 'y' ? 1 : 0;
	}

	if (coredump) {
#ifndef FTN90_IO
		_cleanup();
#endif
		abort();			/* cause a core dump */
	} else {
#ifndef FTN90_IO
		_cleanup();
#endif
		fprintf(stderr,"*** Execution Terminated (%d) ***\n",errno);
		exit(errno);
	}
}
