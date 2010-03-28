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


/* $Header: /home/bos/bk/kpro64-pending/libF77/s_paus.c 1.5 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>		    /* DAG */
#include <string.h>
#include "cmplrs/host.h"

extern void f_exit (void);
static void waitpause(int);	    /* DAG */

void
s_paus(string s, fsize_t n)
{
    fsize_t i;

    fprintf(stderr, "PAUSE ");
    for(i = 0; i < n; ++i) putc(*s++, stderr);
    fprintf(stderr, " statement executed\n");
    if( isatty(fileno(stdin)) ) {
	fprintf(stderr, "To resume execution, type go.  Any other input will terminate job.\n");
	if( getchar()!='g' || getchar()!='o' || getchar()!='\n' ) {
	    fprintf(stderr, "STOP\n");
#ifndef FTN90_IO
	    f_exit();
#endif      
	    exit(0);
	}
    } else {
	fprintf(stderr,
		"To resume execution, execute a   kill -%d %d   command\n",
		SIGTERM, getpid() );
	(void)signal(SIGTERM, waitpause); /* DAG */
	pause();
    }
    fprintf(stderr, "Execution resumes after PAUSE.\n");
}





static void			    /* DAG */
waitpause(int x)
{
    return;
}


