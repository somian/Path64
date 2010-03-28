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


static char USMID[] = "@(#) libcif/cifsetpos.c	30.2	07/26/96 07:19:13";


/*
 * Cif_Setpos positions a compiler input input file to the indicated location.
 */

#define CIF_VERSION 3


#ifdef _ABSOFT
#include "cif.h"
#else
#include <cif.h>
#endif

#include <stdio.h>

#include "cif_int.h"

int Cif_Setpos
#ifdef __STDC__
(int cifd, long pos)
#else
(cifd, pos)
int cifd;		/* CIF file descriptor */
long pos;		/* file position */
#endif
{

	if (cifd < 0 || cifd >= CIF_FT_SIZE || _Cif_filetbl[cifd].form == NOT_A_CIF)
		return (CIF_NOTOPEN);
	else if (_Cif_filetbl[cifd].seek == NO)
		return (CIF_BADREQ);
	else {
		_Cif_filetbl[cifd].ifull = NO;
		if (pos == CIF_FIRST_RECORD) {
			if (_Cif_filetbl[cifd].form == BINARY_CIF)
				pos = BINARY_HDR_LEN;
			else
				pos = 0;
		}
		if (fseek (_Cif_filetbl[cifd].fd, pos, 0))
			return (CIF_SYSERR);
	}
	return (0);
}
