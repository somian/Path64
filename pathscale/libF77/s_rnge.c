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


/* $Header: /home/bos/bk/kpro64-pending/libF77/s_rnge.c 1.5 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <stdio.h>

/* called when a subscript is out of range */

/* Changed this routine so that it can handle procedure names of upto
 * 32 characters and also to be able to print correctly names with '_'
 * in it
 *---ravi---1/13/92
 */
#include "cmplrs/host.h"

extern void s_abort(int32);
static int32  varlen(char *);

void
s_rnge(char *varn, int32 offset, char *procn, int32 line)
{
    register int32 i;
    int32 len;
    
    len=varlen(procn);
    fprintf(stderr,
	    "Subscript out of range on file line %d, procedure ", line);

    /*for(i = 0 ; i < 2 && (*procn!='_' && *(procn+1) != '\0'); ++i)*/

    for(i = 0 ; i < len ; ++i)
	putc(*procn++, stderr);
    fprintf(stderr,
	    ".\nAttempt to access the %d-th element of variable ", offset+1);
#ifdef sgi
    for(i = 0; i < 32 && *varn!=' '; ++i)
	putc(*varn++, stderr);
#else
    for(i = 0 ; i < 6  && *varn!=' ' ; ++i)
	putc(*varn++, stderr);
#endif
    fprintf(stderr, ".\n");
    s_abort(99);
}

static int32
varlen(char *s)
{
    register int32 i;
    /* char name[34]; */

    for(i=0;  i<34 && *s!=' ' && *s!='\0' ; ++i, s++);

    return( i-1 );
}
