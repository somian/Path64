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



#pragma ident "@(#) libf/fio/skipf.c	92.1	06/21/99 10:37:55"

#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	Skip files
 *	dn - unit 
 *	nb - number of files to skip. Negative for skip backwards.
 *	istat - optional 2 word array.
 *		 First word = number of records skipped (not always set)
 *		 Second word = number of files skipped. Only count when
 *			we cross a file boundary.
 *	retstat - optional return status
 */
#define ERET(x) { \
	if (narg > 3) {		\
		*retstat = x;	\
		 goto ret;	\
	} else			\
		_ferr(&cfs, x);	\
}

void
SKIPF(
	_f_int	*unump,
	_f_int8	*nb,
	_f_int	*istat,
	_f_int	*retstat)
{
	register int	narg;
	register unum_t	unum;
	unit		*cup;
	struct fiostate	cfs;
	struct ffp_skipf_s	skip_arg;

#ifdef	_UNICOS
	narg	= _numargs();
#else
	narg	= 4;
#endif
	if (narg > 2) {
		*(istat)	= 0;
		*(istat + 1)	= 0;
	}

	unum	= *unump;

	STMT_BEGIN(unum, 0, T_MISC, NULL, &cfs, cup);
/*
 *	If not connected, do an implicit open.  Abort if the open fails.
 */
	if (cup == NULL) {
		int	ostat;

		cup	= _imp_open(&cfs, SEQ, UNF, unum, 1, &ostat);

		if (cup == NULL) {
			ERET(ostat);
		}
	}

	switch (cup->ufs) {
		case FS_FDC:
			skip_arg.ffp_nfil	= *nb;
			skip_arg.ffp_nrec	= 0;
			if(XRCALL(cup->ufp.fdc, posrtn) cup->ufp.fdc,
				FP_SKIPF, &skip_arg, 2, &cup->uffsw) < 0)
				ERET(errno);	
			if (narg > 2) {
				*(istat+1)	= skip_arg.ffp_nfil;
				*(istat)	= skip_arg.ffp_nrec;
			}
			break;
		default:
			ERET(FENOSKPF);
	}

ret:
	cup->uwrt	= 0;
	cup->uend	= BEFORE_ENDFILE;

	STMT_END(cup, T_MISC, NULL, &cfs);

	return;
}
