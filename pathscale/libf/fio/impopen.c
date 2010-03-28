/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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



#pragma ident "@(#) libf/fio/impopen.c	92.1	06/21/99 10:37:55"

#include <cray/nassert.h>
#ifdef KEY /* Bug 4260 */
#  include <cray/assign.h>
#endif /* KEY Bug 4260 */
#include "fio.h"

/*
 * _ll_implicit_open
 * _imp_open
 * _imp_open77
 *
 *		Performs an implicit open for a unit which was not mentioned
 *		in an OPEN statement before its first I/O (Cray extension).
 *		Upon successful completion, the unit is open and locked.
 *		Since another task might sneak in and open this unit while
 *		_imp_open is being called, the unit should be validated
 *		for proper form and access in the calling routine after return
 *		from _imp_open.  
 *
 *		Side effect:
 *			Stores its return value in css->f_cu
 *
 *		Return value:
 *			The unit pointer on success.
 *			NULL if error, with error status returned in *errstat.
 */

unit *
_ll_implicit_open(
	FIOSPTR	css,		/* Fortran I/O statement state */
	int	acc,		/* SEQ or DIR */
	int	form,		/* FMT or UNF */
	unum_t	unum,		/* Unit number */
	int	errflag,	/* Set if error recovery */
	int	*errstat,	/* Gets error status if error recovery.
				   May be NULL if errflag == 0 */
	int	isf90)		/* =0 iff cf77 style I/O is requested */
{
	int	errn;
	olist	a;
	unit	*cup;

	OPENLOCK();		/* prevent any other OPENs or CLOSEs now */

#ifdef KEY /* Bug 4260 */
	/* Before we open the first file in the course of execution, we must
	 * set byte-swapping based on __io_byteswap_value defined by Fortran
	 * main in response to command-line options like -byteswapio */
        __io_byteswap();
#endif /* KEY Bug 4260 */

	errn	= 0;
	cup	= NULL;

	if (acc == DIR) {
		errn	= FECONNDA;
		goto done;
	}	
		
	if (!GOOD_UNUM(unum)) {
		errn	= FEIVUNIT;
		goto done;
	}	

	cup	= _get_cup(unum);

/*
 *	If the unit is connected, another task must have opened it while
 *	_imp_open was being called, and before _openlock was locked.
 *	Skip the open processing if the unit is already open.
 */
	if (cup == NULL) {

		a.oerr		= errflag;	/* Set error recovery	 */
		a.ounit		= unum;		/* Fortran unit number   */
		a.ofile		= NULL;		/* No file name          */
		a.ofilelen	= 0;		/*    or length          */
		a.ostatus	= OS_UNKNOWN;
		a.oaccess	= OS_SEQUENTIAL;
		a.oform		= (form == FMT) ? OS_FORMATTED : OS_UNFORMATTED;
		a.orecl		= 0;
		a.oposition	= OS_ASIS;
		a.oaction	= OS_ACTION_UNSPECIFIED;
		a.oblank	= OS_NULL;
		a.odelim	= OS_NONE;
		a.opad		= OS_YES;
  
		errn	= _f_open(css, &cup, &a, isf90);	
	}

done:

	OPENUNLOCK();

	if (errn != 0) {		/* Handle an error condition */
		if (cup != NULL)
			_release_cup(cup);

		if (errflag) {
			*errstat	= errn;
			return(NULL);
		}

		_ferr(css, errn, unum);	/* FEIVUNIT needs unum as 2nd arg */
	}

	css->f_cu	= cup;
	return(cup);
}

/*
 *	_implicit_open will be phased out in favor of _imp_open and _imp_open77
 */
unit *
_implicit_open(int acc, int form, unum_t unum, int errflag, int *errstat)
{
	FIOSPTR css;
	GET_FIOS_PTR(css);
	return(_ll_implicit_open(css, acc, form, unum, errflag, errstat, 1) );
}

/*
 *	_imp_open implicitly opens a unit which by default uses Fortran 90
 *	style formatted I/O.
 */ 
unit *
_imp_open(FIOSPTR css, int acc, int form, unum_t unum, int errflag,
	  int *errstat)
{
	return(_ll_implicit_open(css, acc, form, unum, errflag, errstat, 1) );
}

/*
 *	_imp_open77 implicitly opens a unit which by default uses cf77
 *	style formatted I/O.
 */ 
unit *
_imp_open77(FIOSPTR css, int acc, int form, unum_t unum, int errflag,
	    int *errstat)
{
	return(_ll_implicit_open(css, acc, form, unum, errflag, errstat, 0) );
}
