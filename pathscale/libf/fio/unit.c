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



#pragma ident "@(#) libf/fio/unit.c	92.1	06/18/99 18:38:26"
#include <errno.h>
#include <foreign.h>
#include <fortran.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	UNIT function status values
 */
#define UNIT_PDONE	(-2.0)	/* Partial record read complete, data remains */
#define UNIT_DONE	(-1.0)	/* Operation completed */
#define UNIT_EOF	(0.0)	/* EOF on BUFFER IN */
#define UNIT_PERROR	(1.0)	/* Partially recovered error */
#define UNIT_ERROR	(2.0)	/* Unrecovered Error */

/*
 *	_UNIT_
 *
 *		Wait for completion of BUFFER IN/OUT and return a status.
 *
 *      Return value:
 *
 *	   	-2.0    Partial record read complete, data remains
 *              -1.0    Operation complete
 *               0.0    End of file
 *               1.0    Partially recovered error
 *               2.0    Unrecovered error
 *
 *		Undocumented feature: S2 is assigned on exit the specific error
 *		code for the previous BUFFER IN or BUFFER OUT statement for
 *		FFIO files when value 2.0 is returned by the function.
 *
 *	Define duplicate entry points
 *
 *		UNIT	- if user declares it EXTERNAL
 *		@UNIT	- if user declares it INTRINSIC on CRAY-2 systems
 *		$UNIT	- if user declares it INTRINSIC on CX/CEA systems
 *              _UNIT   - if user declares it INTRINSIC with CF77 6.0.0.3 or
 *                         previous on the T3D (obsolete)
 */
#ifdef	_UNICOS
#pragma _CRI duplicate _UNIT_ as UNIT
#pragma _CRI duplicate _UNIT_ as $UNIT
#ifdef _CRAYMPP
#pragma _CRI duplicate _UNIT_ as _UNIT
#endif
#endif	/* _UNICOS */

_f_real
_UNIT_(_f_int *unump)
{
	register unum_t	unum;
	long		s2ret;		/* value to be returned in S2 */
	_f_real		status;
	unit		*cup;
	struct fiostate	cfs;

	s2ret	= 0;
	unum	= *unump;

	STMT_BEGIN(unum, 0, T_UNIT, NULL, &cfs, cup);	/* lock the unit */
/*
 *	If not connected, do an implicit open.  Abort if the open fails.
 */
	if (cup == NULL)
		cup	= _imp_open(&cfs, SEQ, UNF, unum, 0, NULL);

	if (cup->ufs == FS_AUX)
		_ferr(&cfs, FEMIXAUX);

/*
 *	According to the file structure make the appropriate call
 *	to check file status.  File status routines are file
 *	structure dependent.
 */
	cup->unitchk	= 1;	/* indicate that UNIT has now been called */

	WAITIO(cup, {});

	if (cup->uerr) {
		status	= UNIT_ERROR;
		/*
		 * This undocumented behavior of returning the error number
		 * in S2 is preserved for now.
		 */
		s2ret	= cup->uffsw.sw_error;
		goto done;
	}
	else if (!cup->uwrt && cup->uend) {
		status	= UNIT_EOF;
		goto done;
	}
	else if (cup->ufs == FS_FDC         &&	FFSTAT(cup->uffsw) ==  FFCNT &&
		 (cup->uflagword & FFC_REC) && !cup->uwrt                    &&
		 cup->urecmode == PARTIAL) {

		status	= UNIT_PDONE;
		goto done;
	}
	status	= UNIT_DONE;

done:
	STMT_END(cup, T_UNIT, NULL, &cfs);	/* unlock the unit */

#ifdef	_CRAY1
        (void) _sets2(s2ret);
#endif

        return( (_f_real) status);
}
