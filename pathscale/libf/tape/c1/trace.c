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


static char USMID[] = "@(#) libf/tape/c1/trace.c	92.0	10/08/98 14:30:10";

/*
 *	f_trace - enter a line into trace file
 */

#ifdef TRACE

#include	<stdio.h>

#define		TRACESZ		4*4096	

char *tracefn  = "libio.trace";
FILE *tracelog = NULL;
char *tracend  = "<<END>>";

f_trace(s, p1, p2, p3)

char	s[];

{
	int ick;

	if ( tracelog == NULL ) {
		(void) unlink(tracefn);
		tracelog = fopen(tracefn,"w+");
		if ( tracelog == NULL ) {
			printf(" trace open failed \n");
			abort();
		}
	}

	fprintf(tracelog,"%8.8s %22.22o %22.22o %22.22o\n",
		s, p1, p2, p3 );
					
	if ( (TRACESZ != 0) && (tracelog != stdout) ) 
		if (ftell(tracelog) > TRACESZ )
			fseek(tracelog, 0, 0);

	if ( tracelog != stdout ) {
		ick = ftell(tracelog);
		fprintf(tracelog,"%8.8s %22.22o %22.22o %22.22o\n",
			tracend, 0, 0, 0);
		fseek(tracelog, ick, 0);
	}

	fflush(tracelog);

}

#endif

