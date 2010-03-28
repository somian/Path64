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


static char USMID[] = "@(#) libf/tape/c1/gettp.c	92.0	10/08/98 14:30:10";

#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include "fio.h"

#define ERET(errcd)	{ *istat=errcd; goto ret; }
/*
 *	Get tape position
 */

void
GETTP(_f_int *unump, _f_int *len, _f_int8 *pa, _f_int *synch, _f_int *istat)
{
	int  ss;
	unit *cup;
	struct ffc_gettp_s gtptr;

	STMT_BEGIN(*unump, 0, T_TAPE, NULL, NULL, cup);
/*
 *	If not connected, do an implicit open.  
 */
	if (cup == NULL) {
		cup = _implicit_open(SEQ, UNF, *unump, 1, &ss);
		if (cup == NULL)
			ERET(ss);
	}

	*istat = 0;

	switch(cup->ufs) {
		case FS_FDC:
#ifdef __mips
			gtptr.ffc_pa = (long long *)pa;
#else
			gtptr.ffc_pa = (long *)pa;
#endif
			gtptr.ffc_glen = *len;
			gtptr.ffc_synch = *synch;
			ss = XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc,
				FC_GETTP, &gtptr, &cup->uffsw);
			if (ss < 0)
				*istat = cup->uffsw.sw_error;
			break;
		default:
			ERET(FECONNTP);
	}

ret:
	STMT_END(cup, T_TAPE, NULL, NULL);
	return;

}

