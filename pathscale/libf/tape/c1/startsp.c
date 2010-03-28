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


static char USMID[] = "@(#) libf/tape/c1/startsp.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <liberrno.h>
#include <ffio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fio.h"

/*
 *	STARTSP - Begins special processing at EOV
 *
 *      unump - name or unit number of file
 *
 *	istat  - status
 *	  0   OK
 *	  nonzero - Not OK
 */

void
STARTSP(_f_int *unump, _f_int *istat)
{
	register int	ret;
	FIOSPTR		css;
	unit		*cup;

	GET_FIOS_PTR(css);
	STMT_BEGIN(*unump, 0, T_TAPE, NULL, css, cup);

	if (cup == NULL)
                _ferr(css, FENOTOPN);

	*istat	= 0;

        if (cup->ufs == FS_FDC) {
		ret	= XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc,
				FC_STARTSP, 0, &cup->uffsw);	
		if (ret < 0)
			*istat	= cup->uffsw.sw_error;
		else {
			cup->uwrt	= NO;
			cup->uspcproc	= 1;
		}
	}
	else
                _ferr(css, FECONNTP);

	STMT_END(cup, T_TAPE, NULL, css);

	return;
}
