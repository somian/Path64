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



#pragma ident "@(#) libf/fio/endfile.c	92.2	06/18/99 18:41:40"

#include <errno.h>
#include <foreign.h>
#include <unistd.h>
#ifdef	_ABSOFT
#include "ac_sysdep.h"
#else
#include <sys/unistd.h>
#endif
#include "fio.h"
  
/*
 *	_EOFW	Fortran endfile
 */
#ifdef	_UNICOS
#pragma _CRI duplicate _EOFW as $EOFW
#endif

int
_EOFW(
	_f_int	*unump,		/* Fortran unit number */
	_f_int	*iostat,	/* IOSTAT= variable address, or NULL */
	int	errf)		/* 1 if ERR= specifier is present */
{
	register unum_t	unum;
	register int	errn;		/* nonzero when error is encountered */
	unit		*cup;
	struct	ffsw	fst;
	struct fiostate	cfs;
  
#if	defined(_ABSOFT) && (defined(TARGET_MAC_OS) || defined(TARGET_NT))
	mrwe_check();
#endif

	errn	= 0;
	unum	= *unump;

	STMT_BEGIN(unum, 0, T_ENDFILE, NULL, &cfs, cup); /* lock the unit */

	if (!GOOD_UNUM(unum)) {
		errn	= FEIVUNIT;	/* Invalid unit number */
		goto endfile_done;
	}
  
/*
 *	ENDFILE on unopened unit is OK, and does nothing.
 */
        if (cup == NULL)
                goto endfile_done;

 	if (cup->pnonadv) {		/* There is a current record */
 		if (cup->uwrt) {
 			errn	= _nonadv_endrec(&cfs, cup);
 			if (errn != 0)
 				goto endfile_done;
 		}
 		cup->pnonadv	= 0;	/* Flag no current record */
 	}

	cup->urecpos	= 0;

	if ((cup->uaction & OS_WRITE) == 0) {
		errn	= FENOWRIT;	/* No write permission */
		goto endfile_done;
	}

	if (cup->useq == 0) {	/* If file opened for direct access */
		errn	= FEENDFIV;	/* ENDFILE not allowed on dir. acc. */
		goto endfile_done;
	}

	cup->uwrt	= 1;

	if (cup->uend && !cup->umultfil && !cup->uspcproc) {
		errn	= FEENAFEN;	/* ENDFILE after EOF */
		goto endfile_done;
	}
  
	switch (cup->ufs) {
  
	case FS_FDC:
/*
 *		See if the layer we are talking to can represent an
 *		EOF.  If not, write an EOD.  If EOFs are not permitted, then
 *		multi-files (muliple EOFs) are not allowed in the file.
 */
		if (cup->umultfil) {

			/*
			 * Replace a previously encountered logical endfile
			 * record with a physical endfile record.
			 */
			if ((cup->uend == LOGICAL_ENDFILE) && !(cup->uspcproc)){
				if (XRCALL(cup->ufp.fdc, weofrtn)cup->ufp.fdc,
					&fst) < 0) {
                                        errn	= fst.sw_error;
					goto endfile_done;
				}
				cup->uend	= PHYSICAL_ENDFILE;
			}

			if (XRCALL(cup->ufp.fdc, weofrtn)cup->ufp.fdc,
				&fst) < 0) {
				errn	= fst.sw_error;
				goto endfile_done;
			}

			cup->uend	= PHYSICAL_ENDFILE;
		}
		else {
			if (XRCALL(cup->ufp.fdc, weodrtn)cup->ufp.fdc, &fst) < 0)
				errn	= fst.sw_error;

			cup->uend	= LOGICAL_ENDFILE;
		}
		break;
 
	case FS_TEXT:
	case STD:		
		cup->uend	= LOGICAL_ENDFILE;
		break;

	default:
		errn	= FEINTFST;	/* Unknown file structure */
	}
  
endfile_done:
	if (iostat != NULL)
		*iostat	= errn;
	else
		if (errn != 0 && (errf == 0))
			_ferr(&cfs, errn, unum);/* Pass unum to _ferr
						 * in case of FEIVUNIT error */

	STMT_END(cup, T_ENDFILE, NULL, &cfs);	/* unlock the unit */

	errn	= (errn != 0) ? IO_ERR : IO_OKAY;/* 1 if error; 0 if no error */

	return(CFT77_RETVAL(errn));
}
