/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fort/pause.c	92.1	06/24/99 10:18:36"
#include <errno.h>
#include <fortran.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fio.h"

#define MAX_PAS_LEN 80	/* Maximum message length for PAUSE */
#define PAUSESIG    15	/* Signal used to restart a PAUSEd process */

/*
 *	$PAUSE	CF77 entry point to process Fortran PAUSE statement
 *
 *	_PAUSE	F90 entry point to process Fortran PAUSE statement
 *
 *	Argument
 *
 *		s	Fortran character string for the optional "stop code".
 *			Note that Fortran accepts a digit string or a 
 *			character string for the stop code.  The compiler
 *			converts a digit string (integer) to a character
 *			string before calling _PAUSE/$PAUSE.
 *
 *			CF77 5.0 and later passes an FCD with 0 length
 *			if the user omitted the stop code.
 *
 *			F90 passes an FCD for ' ' if the user omitted the
 *			stop code.
 */
#ifdef _UNICOS
#pragma _CRI duplicate _PAUSE as $PAUSE
#endif

void
_PAUSE(_fcd s)
{
	int	len;
	void	_waitpause();
	char	*msg;

#ifdef	_UNICOS
	if (_numargs() == 0) {
		msg	= "";
		len	= 0;
	}
	else 
#endif
	{
		msg	= _fcdtocp(s);
		len	= _fcdlen(s);

		if (len > MAX_PAS_LEN)
			len	= MAX_PAS_LEN;
	}

	(void) fprintf(errfile, " PAUSE %.*s\n", len, msg);
	(void) fprintf(errfile, " To resume execution, type:  ");

	if (isatty(fileno(stdin))) {

		(void) fprintf(errfile, "go\n");
		(void) fprintf(errfile,
			" Any other input will terminate the program\n");

		if (getchar() != 'g' || getchar() != 'o' || getchar() != '\n') {
			(void) fprintf(errfile, " STOP\n");
			exit(0);
		}
	}
	else {
		(void) fprintf(errfile, "kill -%d %d\n", PAUSESIG, getpid());
		signal(PAUSESIG, _waitpause);
		pause();	/* The pause that refreshes */
	}

	(void) fprintf(errfile, " Execution resumed after PAUSE\n");
}

void
_waitpause()
{
	return;
}
