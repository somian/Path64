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



#pragma ident "@(#) libf/fio/numblks.c	92.1	06/21/99 10:37:21"
 
#include <stdio.h>
#include <foreign.h>
#include <errno.h>
#include <liberrno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fio.h"
 
#ifdef	_UNICOS
_f_int8
NUMBLKS(
#elif defined(__mips) || defined(_LITTLE_ENDIAN)
_f_int8
numblks_(
#else
_f_int
numblks_(
#endif
_f_int *unump
)
{
	int 		n;
#if	defined(_UNICOS) || defined(__mips) || defined(_LITTLE_ENDIAN)
	_f_int8		retval;
#else
	int 		retval;
#endif
	unum_t 		unum;
	unit		*cup;
	struct stat 	buf;
	struct fiostate	cfs;

	unum	= *unump;

	STMT_BEGIN(unum, 0, T_NUMBLKS, NULL, &cfs, cup);

	if (cup == NULL && !GOOD_UNUM(unum))
		_ferr(&cfs, FEIVUNIT, unum);	/* invalid unit number */

	if (cup == NULL)
		retval	= -1;			/* unit is not open */
	else if (cup->usysfd == -1)
		retval	= 0;			/* file is not disk-resident */
	else {
		n	= fstat(cup->usysfd, &buf);
		if (n < 0)
			_ferr(&cfs, errno);

		/* Convert bytes to blocks */

		retval	= (buf.st_size + 4095) >> 12;
	}

	STMT_END(cup, T_NUMBLKS, NULL, &cfs);	/* unlock the unit */
	return(retval);
}
