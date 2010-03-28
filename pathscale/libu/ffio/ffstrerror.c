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


#pragma ident "@(#) libu/ffio/ffstrerror.c	92.3	10/11/99 15:30:43"

#include <liberrno.h>
#include <nl_types.h>
#include <string.h>
#include <stdio.h>
#if	defined(_LITTLE_ENDIAN)
#include <cray/nlcatmsg.h>
#endif

char	__dfltmsg[32];	/* Default message */

/*
 *	ffstrerror
 *
 *	char *ffstrerror(int);
 *
 *	Returns a pointer to an error message string associated with the
 *	specified error number (returned previously via an ff* routine),
 *	suitable for printing, etc.  The message string should not be
 *	overwritten.
 */

char *ffstrerror(int errnum)
{
	char	*msgptr;	/* Pointer to error message string */

	(void) sprintf(__dfltmsg, "Unknown error %d", errnum);

	if (errnum < BASE) {	/* If not a library error message */
		msgptr	= strerror(errnum);

		if (msgptr == (char *) NULL)
			msgptr	= __dfltmsg;
	}
	else {
		nl_catd	mcfd;		/* Message catalog file descriptor */

		mcfd	= catopen(FEMCN, 0);
		msgptr	= catgets(mcfd, NL_MSGSET, errnum, __dfltmsg);
	}

	return(msgptr);
}
