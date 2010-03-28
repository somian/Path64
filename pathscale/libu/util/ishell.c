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


#pragma ident "@(#) libu/util/ishell.c	92.2	10/29/99 21:40:31"
#include <errno.h>
#include <fortran.h>
#include <stddef.h>
#include <stdlib.h>

/*
 *	ISHELL - Fortran callable routine to send a command
 *		to the shell.
 *		
 *         INPUT:  cmdargs--type character argument or type
 *                          other containing character data that
 *                          ends with a NULL character.
 *	   OUTPUT: integer variable containing status returned by
 *                 the system() call.  If system() returns -1,
 *                 the value of -errno is returned.
 */

#ifdef	_UNICOS
_f_int
ISHELL(_fcd cmdarg)
{
	char	*cptr;
	int	ret;
	int	fcdflag;

	/* Check number of arguments */

	if (_numargs() < 1)
		return( (_f_int) -1);

	/* IS cmdarg an _fcd ? */
	fcdflag	= 0;
#ifdef	_ADDR64
	if (_numargs() * sizeof(long) == sizeof(_fcd)) 
#else
	if (_isfcd(cmdarg))
#endif
		fcdflag	= 1;
  
	/* Convert argument to C character string */

	if (fcdflag) {		/* If Fortran character */
		cptr	= _fc_acopy(cmdarg);

		if (cptr == NULL)
			return( (_f_int) -1);
	}
	else
		cptr	= _fcdtocp(cmdarg);

	/* Run command using system() */

	ret	= system(cptr);

	if (ret == -1)
		ret	= -errno;

	if (fcdflag)
		free(cptr);

	return( (_f_int) ret);
}

#endif	/* _UNICOS */

#ifndef	_UNICOS
#include <string.h>
_f_int
ishell_(char* cmdcptr, int cmdlen)
{
	char	*cptr;
	int	ret;

	cptr = malloc(cmdlen + 1);
	if (cptr == NULL)
		return( (_f_int) -1);

	(void) memcpy(cptr, cmdcptr, cmdlen);
	cptr[cmdlen] = '\0';

	ret	= system(cptr);

	if (ret == -1)
		ret	= -errno;

	free(cptr);

	return( (_f_int) ret);
}
#endif	/* ! _UNICOS */
