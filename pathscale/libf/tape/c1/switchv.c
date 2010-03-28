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


static char USMID[] = "@(#) libf/tape/c1/switchv.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include "fio.h"

#define ERET(x) {				\
			if (narg > 1) {		\
				*istat	= x;	\
				goto done;	\
			}			\
			_ferr(css, x);		\
		}
/*
 *	CLOSEV - Switches tape volume
 *
 *      unump - name or unit number of file
 *
 *	istat
 *         0  OK
 *         nonzero- No error
 */

void
CLOSEV(_f_int *unump, _f_int *istat)
{
	register int	narg;
	register int	ret;
	FIOSPTR		css;
	unit		*cup;

#ifdef __mips
	narg = 2;
#else
	narg	= _numargs();

	if (narg > 1)
	
#endif
		*istat	= 0;

	GET_FIOS_PTR(css);
	STMT_BEGIN(*unump, 0, T_TAPE, NULL, css, cup);

	if (cup == NULL)
		_ferr(css, FENOTOPN);

        if (cup->ufs == FS_FDC ) {
		ret	= XRCALL(cup->ufp.fdc, fcntlrtn)cup->ufp.fdc,
				FC_CLOSEV, 0, &cup->uffsw);
		if (ret < 0)
			ERET(cup->uffsw.sw_error);
	}
	else
                _ferr(css, FECONNTP);

done:
	cup->uend	= BEFORE_ENDFILE;

	STMT_END(cup, T_TAPE, NULL, css);

	return;
}
