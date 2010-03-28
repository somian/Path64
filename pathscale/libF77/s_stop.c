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


/* $Header: /home/bos/bk/kpro64-pending/libF77/s_stop.c 1.5 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmplrs/host.h"

extern void f_exit (void);

void
s_stop(string s, fsize_t n)
{
    fsize_t i;
    int32 return_code=0;
    int convert=1;

/* Do not print this if no string ( this routine gets a '\0' because of 
 * a restriction is as1 ) is passed to stop.
 * ---ravi---12/16/91
 */
    if(*s && n>0) {
	fprintf(stderr, "STOP ");
	for(i = 0; i < n; ++i)
	 {
	 if (convert && isdigit(*s)) 
		{
		return_code=return_code*10+(*s-'0');
		}
	 else
		{
		convert=0;
		}
	 putc(*s++, stderr);
	 }
	fprintf(stderr, " statement executed\n");
    }
#ifndef FTN90_IO
    f_exit();
#endif      
    exit(return_code);
}
