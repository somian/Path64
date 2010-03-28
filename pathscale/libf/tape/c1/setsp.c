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


static char USMID[] = "@(#) libf/tape/c1/setsp.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include "fio.h"

#ifndef __mips
extern void _eov_load();
#endif

/*
 *	SETSP - Initializes/terminates special EOV processing
 *
 *	unump - name or unit # of file
 *
 *	iflag
 *	  0 - Turn BOV/EOV processing OFF
 *	  nonzero - Turn BOV/EOV processing ON
 *
 *      istat - status
 *	  0 - OK
 *        nonzero - error
 */

void
SETSP(_f_int *unump, _f_int *iflag, _f_int *istat)
{
	register int	ret;
	FIOSPTR		css;
	unit		*cup;

	GET_FIOS_PTR(css);
	STMT_BEGIN(*unump, 0, T_TAPE, NULL, css, cup);
/*
 *	If not connected, do an implicit open.  Abort if the open fails.
 */
	if (cup == NULL)
		cup	= _imp_open(css, SEQ, UNF, *unump, 0, NULL);

	*istat	= 0;

	if (cup->ufs == FS_FDC) {
#ifndef __mips
		_eov_load(1);	/* Call a routine that provides hard */
				/* references to eov routines. */
#endif
		ret	= XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc,
				FC_SETSP, (void *)*iflag, &cup->uffsw);
		if (ret < 0)
			*istat	= cup->uffsw.sw_error;
	}
	else 
		_ferr(css, FECONNTP);	

	STMT_END(cup, T_TAPE, NULL, css);

	return;
}
