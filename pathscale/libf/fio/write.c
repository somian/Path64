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



#pragma ident "@(#) libf/fio/write.c	92.2	06/21/99 10:37:55"

/*
 *	write words
 */

#include <errno.h>
#include <foreign.h>
#include <fortran.h>
#include <liberrno.h>
#include "fio.h"

#define UBC	4	/* argument number for optional ubc parameter */
#define STATUS	5	/* argument number for optional status parameter */

#undef	WRITE

static void __WRITE();

/*
 *	Write words, full record mode
 */

_f_int
WRITE(
	_f_int 	*unump,
	_f_int	*uda,
	_f_int	*wordsp,
	_f_int	*ubc,
	_f_int	*status)
{
	_f_int	locubc, *ubcp;
	_f_int	*statp;

	if (_numargs() < UBC) {
		locubc	= 0;
		ubcp	= &locubc;
	}
	else
		ubcp	= ubc;

	statp	= (_numargs() < STATUS) ? NULL : status;

	__WRITE(FULL, unump, uda, wordsp, ubcp, statp);

	return(0);
}

/*
 *	Write words, partial record mode
 */

void
WRITEP(
	_f_int	*unump,
	_f_int	*uda,
	_f_int	*wordsp,
	_f_int	*ubc,
	_f_int	*status)
{
	_f_int	locubc, *ubcp;
	_f_int	*statp;

	if (_numargs() < UBC) {
		locubc	= 0;
		ubcp	= &locubc;
	}
	else
		ubcp	= ubc;

	statp	= (_numargs() < STATUS) ? NULL : status;

	__WRITE(PARTIAL, unump, uda, wordsp, ubcp, statp);

	return;
}

static void
__WRITE(
	int	fulp,
	_f_int	*unump,
	_f_int	*uda,
	_f_int	*wordsp,
	_f_int	*ubc,
	_f_int	*status)
{
	register int	ret;
	register int	errn;
	int		wstat;
	int		wubc;
	register unum_t	unum;
	unit		*cup;
	type_packet	tip;
	struct fiostate	cfs;

	unum	= *unump;
	wubc	= *ubc;
	errn	= 0;

	STMT_BEGIN(unum, 0, T_WSU, NULL, &cfs, cup);
/*
 *	If not connected, do an implicit open.
 */
	if (cup == NULL) {
		int	ostat;

		cup	= _imp_open(&cfs, SEQ, UNF, unum, (status != NULL),
				&ostat);

		if (cup == NULL) {
			errn	= ostat;
			goto done;
		}
	}

	if (!cup->ok_wr_seq_unf) {
		errn	= _get_mismatch_error(1, T_WSU, cup, &cfs);
		goto done;
	}

	cup->uwrt	= 1;

	if (cup->uend) {
		/*
		 * If positioned after an endfile, and the file does not
		 * support multiple endfiles, a write is invalid.
		 */
		if (!cup->umultfil && !cup->uspcproc) {
			errn	= FEWRAFEN;
			goto done;
		}
		/*
		 * If a logical endfile record had just been read,
		 * replace it with a physical endfile record before
		 * starting the current data record.
		 */
		if ((cup->uend == LOGICAL_ENDFILE) && !(cup->uspcproc)) {
			if (XRCALL(cup->ufp.fdc, weofrtn)cup->ufp.fdc,
			 &cup->uffsw) < 0) {
				errn	= cup->uffsw.sw_error;
				goto done;
			}
		}
		cup->uend	= BEFORE_ENDFILE;
	}

	tip.type90      = DVTYPE_TYPELESS;
        tip.type77      = -1;
        tip.intlen      = sizeof(long) << 3;
        tip.extlen      = tip.intlen;
        tip.elsize      = sizeof(long);
        tip.cnvindx     = 0;
        tip.count       = *wordsp;
        tip.stride      = 1;

	ret	= _fwwd(cup, uda, &tip, fulp, &wubc, (long *) NULL, &wstat);

	if ( ret == IOERR ) {
		errn	= errno;
		goto done;
	}

	if (status != NULL)
		*status	= 0;

done:
	if (errn != 0) {
		if (status == NULL)
			_ferr(&cfs, errn);	
		else
			*status	= errn;	
	}

	*ubc	= wubc;

	STMT_END(cup, TF_WRITE, NULL, &cfs);

	return;
}
