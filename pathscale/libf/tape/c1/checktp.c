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


static char USMID[] = "@(#) libf/tape/c1/checktp.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <foreign.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	CHECKTP - Checks tape I/O status
 *
 *      unump - unit number of file
 *
 *	istat
 *	  -1  No status
 *         0  EOV
 *	   1  tape off reel
 *         2  tape mark detected
 *	   3  blank tape detected
 *
 *      icbuf 
 *	   Number of blocks in library buffers 
 */

void
CHECKTP(long *unump, long *istat, long *icbuf)
{
	register int	ret;
	unit		*cup;
	FIOSPTR		css;
	struct ffc_chktp_s chktp;

	GET_FIOS_PTR(css);
	STMT_BEGIN(*unump, 0, T_TAPE, NULL, css, cup);

	if (cup == NULL)
		_ferr(css, FENOTOPN);

	switch(cup->ufs) {
		case FS_FDC:
			ret	= XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc,
					FC_CHECKTP, &chktp, &cup->uffsw);
			if (ret < 0) 
				_ferr(css, cup->uffsw.sw_error);
			*istat	= chktp.stat;
			*icbuf	= chktp.libblk;
			break;
		default:
                	_ferr(css, FECONNTP);
	}

	STMT_END(cup, T_TAPE, NULL, css);

	return;
}
