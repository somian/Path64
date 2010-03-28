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


static char USMID[] = "@(#) libcif/cifgetpos.c	30.2	07/26/96 07:19:13";


/*
 * Cif_Getpos returns the current position of a Compiler Information File.
 */

#define CIF_VERSION 3

#ifdef _ABSOFT
#include "cif.h"
#else
#include <cif.h>
#endif


#include <stdio.h>

#include "cif_int.h"

long Cif_Getpos
#ifdef __STDC__
(int cifd)
#else
(cifd)
int cifd;		/* CIF file descriptor */
#endif
{

	long pos;

	if (cifd < 0 || cifd >= CIF_FT_SIZE || _Cif_filetbl[cifd].form == NOT_A_CIF)
		return (CIF_NOTOPEN);
	else if (_Cif_filetbl[cifd].seek == NO)
		return (CIF_BADREQ);
	else if ((pos = ftell (_Cif_filetbl[cifd].fd)) < 0)
		return (CIF_SYSERR);
	return (pos);
}
