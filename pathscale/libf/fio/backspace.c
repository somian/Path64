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



#pragma ident "@(#) libf/fio/backspace.c	92.1	06/18/99 15:49:57"

#include <errno.h>
#include <foreign.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	_BACK   Fortran backspace
 */

#if	_CRAY 
#pragma _CRI duplicate _BACK as $BACK	/* For cf77 */
#endif

int
_BACK(
	_f_int  *unump,			/* Fortran unit number */
	_f_int  *iostat,		/* IOSTAT= variable address, or NULL */
	int     errf)			/* 1 if ERR= specifier is present */
{
	register int	errn;		/* nonzero when error is encountered */
	register unum_t	unum;
	unit		*cup;
	struct fiostate	cfs;

	errn	= 0; 
	unum	= *unump; 

	STMT_BEGIN(unum, 0, T_BACKSPACE, NULL, &cfs, cup); /* lock the unit */

	if (!GOOD_UNUM(unum)) {
		errn	= FEIVUNIT;	/* Invalid unit number */
		goto backspace_done;
	}

/*
 *	BACKSPACE on unopened unit is OK, and does nothing.  For opened units,
 *	call the low level backspace routine.
 */
	if (cup == NULL) 
		goto backspace_done;

 	if (cup->pnonadv) {		/* There is a current record */
 		if (cup->uwrt) {

 			errn	= _nonadv_endrec(&cfs, cup);

 			if (errn != 0)
 				goto backspace_done;
 		}
 		cup->pnonadv	= 0;	/* Flag no current record */
 	}

	errn	= _unit_bksp(cup);

backspace_done:
	if (iostat != NULL)
		*iostat	= errn;
	else
		if (errn != 0 && (errf == 0))
			_ferr(&cfs, errn, unum);	/* Pass unum to _ferr
						 * in case of FEIVUNIT error */

	STMT_END(cup, T_BACKSPACE, NULL, &cfs);	/* unlock the unit */

	errn	= (errn != 0) ? IO_ERR : IO_OKAY;/* 1 if error; 0 if no error */

	return(CFT77_RETVAL(errn));
}
