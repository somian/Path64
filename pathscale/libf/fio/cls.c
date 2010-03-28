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



#pragma ident "@(#) libf/fio/cls.c	92.2	06/18/99 18:41:40"

#include <stdio.h>
#include <cray/nassert.h>
#include <ctype.h>
#include "fio.h"
#include "f90io.h"
#include "fmt.h"
#ifdef	_CRAYMPP
#include <stdarg.h>
#endif


/*
 *	$CLS - routine called to process the Fortran CLOSE statement.
 *
 *	_CLS - obsolete Fortran 90 CLOSE interface routine.
 */
#ifdef	_UNICOS
#pragma _CRI duplicate _CLS as $CLS
#endif
#ifdef	_CRAYMPP
_CLS (
	_f_int	*unump,		/* Pointer to unit identifier   */
	_f_int	*iostat,	/* Pointer to IOSTAT variable   */
	int	*err,		/* Pointer to ERR specifier     */
	...			/* STATUS character descriptor  */
)
#else
int _CLS (
	_f_int	*unump,		/* Pointer to unit identifier   */
	_f_int	*iostat,	/* Pointer to IOSTAT variable   */
	int	*err,		/* Pointer to ERR specifier     */
	_fcd    status		/* STATUS character descriptor  */
)
#endif
{
#ifdef _CRAYMPP
	va_list	args;		/* Variable argument list */
	_fcd    status;		/* STATUS character descriptor  */
#endif
	register int	csta;	/* STATUS= specifier value */
	register int	errn;	/* Error status */
	int		errflag;/* Contains _UERRF/_UIOSTF flag bits */
	long		fstrlen;/* Length of Fortran string */
	char		*fstr;	/* Pointer to Fortran string */
	plock_t		*lockp;	/* Pointer to the unit lock */
	register unum_t	unum;	/* Fortran unit number */
	unit		*cup;	/* Pointer to unit table */
	struct fiostate cfs;
	static	char	keep[] = "KEEP";
	static	char	delete[] = "DELETE";
  
	int 	_string_cmp();	/* String compare routine in libu */

	OPENLOCK();		/* prevent other OPENs or CLOSEs right now */

	unum    = *unump;

	STMT_BEGIN(unum, 0, T_CLOSE, NULL, &cfs, cup);

	errflag		= (iostat != NULL ? _UIOSTF : 0) |
			  (   err != NULL ?  _UERRF : 0);
	errn    = 0;
	csta	= CLST_UNSPEC;
  
	/* Validate STATUS parameter, if specified */
  
#ifdef	_CRAYMPP
	va_start(args, err);
	status	= va_arg(args, _fcd);
	va_end(args);
#endif
	fstr	= _fcdtocp(status);
	fstrlen	= _fcdlen (status);

	if (fstr != NULL) {
		if     (_string_cmp(  keep, fstr, fstrlen))
			csta	= CLST_KEEP;
		else if(_string_cmp(delete, fstr, fstrlen))
			csta	= CLST_DELETE;
		else
			errn	= FECLSTAT;	/* Invalid status parameter */
	}
/*
 *	A CLOSE for a unit which doesn't exist or isn't connected is a no-op.
 */
	if (!GOOD_UNUM(unum) || RSVD_UNUM(unum) || cup == NULL) goto done;

	cup->uflag	= errflag;

	if (csta == CLST_KEEP && cup->uscrtch) 
		errn	= FECLSTIV;	/* Can't KEEP scratch file  */
  
	lockp	= &cup->uiolock;	/* save the unit lock pointer address */

	if (errn == 0)
		errn	= _unit_close(cup, csta, &cfs);	/* low level close */
  
/*
 *	The unit pointer cup normally is not connected after return from
 *	_unit_close.  So cup would point to deallocated memory if units
 *	were allocated dynamically.
 */
	MEM_UNLOCK(lockp);		/* Unlock the unit lock */
  
done:
	OPENUNLOCK();

	if (iostat != NULL)
		*iostat	= errn;
  
	if ((errn != 0) && (errflag == 0))
		_ferr(&cfs, errn);

	STMT_END((unit *)NULL, T_CLOSE, NULL, &cfs); /* unit already unlocked */
  
	errn	= (errn != 0) ? IO_ERR : IO_OKAY;
  
	return(CFT77_RETVAL(errn));
}

/*
 *	_CLOSE - process Fortran-90 CLOSE statement.
 */
int
_CLOSE(struct close_spec_list *c)
{
	assert ( c->version == 0 );

	return(_CLS(c->unit, c->iostat, (int *)c->err, c->status));
}
