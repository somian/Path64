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


#pragma ident "@(#) libu/util/c1/jname.c	92.1	07/07/99 13:22:26"
#include <fortran.h> 
#include <stdlib.h>
#include <string.h>

#define BLANK	((int) ' ')

/*
 *	JNAME 	Returns the name of the job if present in a QSUB. 
 *		Otherwise, it returns the logname.  The result is
 *              left justified and filled with blanks on the right.
 *
 *		May be called as either a function or a subroutine.
 *
 *              As a character function, the reference is:
 *
 *                EXTERNAL JNAME
 *                CHARACTER*10 JNAME, RESULT, NAME
 *                NAME = JNAME(RESULT)
 *
 *              As a noncharacter function, the reference is:
 *
 *                EXTERNAL JNAME
 *                INTEGER RESULT
 *                RESULT = JNAME()
 *
 *              As a subroutine call, the reference is:
 *
 *                CHARACTER*10 JOBNAM
 *                CALL JNAME (JOBNAM)
 *
 *              or
 *
 *                INTEGER RESULT
 *                CALL JNAME (RESULT)
 *
 */
_f_int
JNAME(name)
long	name;
{
 
	long	data;
	char	*qname;

	/*
 	 * retrieve either the QSUB name or the LOGNAME 
	 */

	if ((qname = getenv("QSUB_REQNAME")) == NULL)
		qname	= getenv("LOGNAME");
	/*
 	 * place name in integer for numeric variable target
	 */

	(void) memcpy((char *) &data, qname, sizeof(long));

	/*
 	 * determine if target variable is character
	 */

	if ( _numargs() > 0 ) {

#ifdef  _ADDR64
		if (_numargs() * sizeof(long) >= sizeof(_fcd)) {
#else
		if (_isfcd(name)) {
#endif
			/* destination is a character */
			unsigned int	nlen, qlen;
			char		*cp;
			_fcd		fcdtemp;

			qlen	= strlen(qname);
			fcdtemp = *(_fcd *)&name;
			cp	= _fcdtocp(fcdtemp);
			nlen	= _fcdlen(fcdtemp);

			(void) strncpy(cp, qname, nlen);

			if (qlen < nlen)	/* Pad with blanks */
				(void) memset(cp + qlen, BLANK, nlen - qlen);
		}
		else {			/* Destination is hollerith */
			long	*lp;

			lp	= (long *) (name);
			*lp	= data;
		}
	}

	return (data);
} 
