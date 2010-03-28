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



#pragma ident "@(#) libf/fio/isatty.c	92.1	06/21/99 10:37:55"
 
/*
 *
 * isatty_  - determine if stream is associated with tty (async port)
 *
 * calling sequence:
 *
 *	LOGICAL	isatty, val
 *	INTEGER	lunit
 *	val = isatty (lunit)
 *
 * where:
 *
 *	val will be .TRUE. if lunit is associated with a 'tty'
 *
 * Note: the return value will not indicate if there is an error,
 *       errno should be checked.
 *
 * Entry point __isatty_f90 is called for MIPS f77 or for f90 when
 *      there is no compatibility module.
 *
 * Entry point isattyf90_ is called for MIPS f90 when there is a
 *      compatibility module.
 *
 * Entry point isattyf90_8_ is called for MIPS f90 when there is a
 *      compatibility module.
 *
 */

#include <stdio.h>
#include <foreign.h>
#include <errno.h>
#include <liberrno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fio.h"

extern _f_int __isatty_f90 (_f_int *u);
#ifdef KEY /* Bug 1683 */
static _f_int isattyf90_(_f_int *u);
#else
extern _f_int isattyf90_(_f_int *u);
#endif /* KEY Bug 1683 */
extern _f_int8 isattyf90_8_(_f_int8 *u);

_f_int
__isatty_f90 (_f_int *u)
{
	return isattyf90_(u);
}

#ifdef KEY /* Bug 1683 */
/* Don't pollute the Fortran namespace */
static
#endif /* KEY Bug 1683 */
_f_int
isattyf90_(_f_int *u)
{
	int 		rtrn, errval;
	unum_t 		unum;
	unit		*cup;
	struct fiostate	cfs;

	unum	= *u;
	STMT_BEGIN(unum, 0, T_INQU, NULL, &cfs, cup);

	errval	= 0;
	if (cup == NULL && !GOOD_UNUM(unum))
		_ferr(&cfs, FEIVUNIT, unum);	/* invalid unit number */

	if (cup == NULL)
		errval	= FEIVUNIT;	/* unit is not open */
	else if (cup->usysfd == -1)
		errval	= FEIVUNIT;	/* file is not disk-resident */
	else {
		rtrn	= isatty(cup->usysfd);
	}

	STMT_END(cup, T_INQU, NULL, &cfs);	/* unlock the unit */

	if (errval!=0)
		errno	= errval;
	return (_btol(rtrn));
}

_f_int8
isattyf90_8_(_f_int8 *u)
{
	int 		rtrn, errval;
	unum_t 		unum;
	unit		*cup;
	struct fiostate	cfs;

	unum	= *u;
	STMT_BEGIN(unum, 0, T_INQU, NULL, &cfs, cup);

	errval	= 0;
	if (cup == NULL && !GOOD_UNUM(unum))
		_ferr(&cfs, FEIVUNIT, unum);	/* invalid unit number */

	if (cup == NULL)
		errval	= FEIVUNIT;	/* unit is not open */
	else if (cup->usysfd == -1)
		errval	= FEIVUNIT;	/* file is not disk-resident */
	else {
		rtrn	= isatty(cup->usysfd);
	}

	STMT_END(cup, T_INQU, NULL, &cfs);	/* unlock the unit */

	if (errval!=0)
		errno	= errval;
	return (_btol(rtrn));
}
