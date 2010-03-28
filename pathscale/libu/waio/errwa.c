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


#pragma ident "@(#) libu/waio/errwa.c	92.1	06/23/99 13:55:03"

#include <stdio.h>
#include <liberrno.h>
#include "waio.h"

#ifdef	_CRAY2
extern	FILE	*errfile;
#else
#define	errfile	stderr
#endif

/*
 *	_errwa - Process errors in word-addressable package
 */
void
_errwa(routine, text, fp, errn)
char	*routine;	/* Name of routine finding error	*/
char	*text;		/* General text				*/
WAFIL	*fp;		/* WAFIL table entry 			*/
int	errn;		/* System or Fortran error code		*/
{
	if (fp->wa_idn[0] == '\0') 	/* This is a Fortran -s bin file */
		_lerror(_LELVL_ABORT, errn);
	else {
		(void) fprintf (errfile, " \n");
		(void) fprintf (errfile, "%s: %s file %.8s\n", routine, text,
					fp->wa_idn);
		(void) fprintf (errfile, " \n");
		_lerror(_LELVL_ABORT, errn);
	}
}

/*
 *	_errwa_abort	Process errors when we don't have a valid WAFIL table
 * 			entry.
 */
void
_errwa_abort(errn)
{
	_lerror(_LELVL_ABORT, errn);
}

/*
 *	_errwa_msg	Process errors when we don't have a valid WAFIL table
 * 			entry.
 */
void
_errwa_msg(errn)
{
	_lerror(_LELVL_MSG, errn);
}
