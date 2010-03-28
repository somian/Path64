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


static char USMID[] = "@(#) libf/tape/c1/skipbad.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <foreign.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	skip bad data
 */

void
SKIPBAD(unump, blocks, termcnd)
long *unump, *blocks, *termcnd;
{
	int  ret;
	unit *cup;
	struct ffc_baddata_s bddata;
	
	STMT_BEGIN(*unump, 0, T_TAPE, NULL, NULL, cup);
/*
 *	If not connected, do an implicit open.  Abort if the open fails.
 */
	if (cup == NULL)
		cup = _implicit_open(SEQ, UNF, *unump, 0, NULL);

	switch(cup->ufs) {
		case FS_FDC:
			ret = XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc, 
				FC_SKIPBAD, &bddata, &cup->uffsw);
			if (ret < 0) 
				*termcnd = -cup->uffsw.sw_error;
			else {
				*blocks = bddata.ffc_blocks;
				*termcnd = bddata.ffc_termcnd;
			}	
			break;
		default:
			*termcnd = -FECONNTP;
	}

	STMT_END(cup, T_TAPE, NULL, NULL);
	return;
}
