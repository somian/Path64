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


#pragma ident "@(#) libu/trbk/trbk.c	92.1	06/23/99 18:14:34"
#include <infoblk.h>
#include <signal.h>
#pragma	soft	fflush
#include <stdio.h>
#include <stdlib.h>
#include <cray/signal.h>
#include <cray/mtlock.h>

static DECL_LOCK(traceback_lock)	/* lock word for routine */

extern	int	_trbkdpth;

extern	int	_trbk(int _Fd, ...);

long
TRBK(long *depth)
{
	long	stat;

	MEM_LOCK(&traceback_lock);

	if (_numargs() > 0)	/* If depth specified */
		_trbkdpth	= *depth;
	else {
		char	*env;

        	if (env = getenv("TRBKDPTH"))
                	_trbkdpth	= atoi(env);
	}

	/* Flush all files */

	if (LOADED(fflush))
		(void) fflush(NULL);

	stat	= _trbk(fileno(stderr));

	MEM_UNLOCK(&traceback_lock);

	return(stat);
}

int
sigtrbk(FILE *stream, int depth)
{
	long	stat;
	FILE	*f;

	if (_numargs() > 0)	/* If stream specified */
		f	= stream;
	else
		f	= stderr;

	MEM_LOCK(&traceback_lock);

	if (_numargs() > 1)	/* If depth specified */
		_trbkdpth	= depth;
	else {
		char	*env;

        	if (env = getenv("TRBKDPTH"))
                	_trbkdpth	= atoi(env);
	}

	/* Flush all files */

	if (LOADED(fflush))
		(void) fflush(NULL);

	stat	= _trbk(fileno(f), _sigarea());

	MEM_UNLOCK(&traceback_lock);

	return((int)stat);
}

int
trbk(FILE *stream, int depth)
{
	long	stat;
	FILE	*f;

	if (_numargs() > 0)	/* If stream specified */
		f	= stream;
	else
		f	= stderr;

	MEM_LOCK(&traceback_lock);

	if (_numargs() > 1)	/* If depth specified */
		_trbkdpth	= depth;
	else {
		char	*env;

        	if (env = getenv("TRBKDPTH"))
                	_trbkdpth	= atoi(env);
	}

	/* Flush all files */

	if (LOADED(fflush))
		(void) fflush(NULL);

	stat	= _trbk(fileno(f));

	MEM_UNLOCK(&traceback_lock);

	return((int)stat);
}

long
$TRBK()
{
	long	stat;
	char	*env;

	MEM_LOCK(&traceback_lock);

        if (env = getenv("TRBKDPTH"))
               	_trbkdpth	= atoi(env);

	/* Flush all files */

	if (LOADED(fflush))
		(void) fflush(NULL);

	stat	= _trbk(fileno(stderr));

	MEM_UNLOCK(&traceback_lock);

	return(stat);
}
