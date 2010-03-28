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


static char USMID[] = "@(#) libcif/cifclose.c	30.2	07/26/96 07:19:13";


/*
 * Cif_Close closes an open CIF file.  The file is closed and the CIF open
 * file table entry is cleared.
 */

#define CIF_VERSION 3

#ifdef _ABSOFT
#include "cif.h"
#else
#include <cif.h>
#endif

#if defined(BUILD_OS_DARWIN)
#include <stdlib.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <malloc.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <unistd.h>
#include <stdio.h>

#include "cif_int.h"

int Cif_Close
#ifdef __STDC__
(int cifd, int free_mode)
#else
(cifd, free_mode)
int cifd;		/* CIF file descriptor */
int free_mode;	/* buffer return handling indicator */
#endif
{

	int me;
 
	if (cifd < 0 || cifd >= CIF_FT_SIZE ||_Cif_filetbl[cifd].form == NOT_A_CIF)
		return (CIF_NOTOPEN);
	else {

		switch (_Cif_filetbl[cifd].mode) {

		case CIF_MEM_FIXED:
			me = _Cif_filetbl[cifd].fme;
			if (free_mode == CIF_MEM_FREE) {
				(void) free (_Cif_memarea[me].mbp);
				_Cif_memarea[me].mbp = NULL;
			}
			_Cif_memarea[me].used = NO;
			break;

		case CIF_MEM_MANAGED:
			for (me = _Cif_filetbl[cifd].fme; me != 0; me = _Cif_memarea[me].nme) {
				if (free_mode == CIF_MEM_FREE) {
					(void) free (_Cif_memarea[me].mbp);
					_Cif_memarea[me].mbp = NULL;
				}
				_Cif_memarea[me].used = NO;
			}
			break;

		default:
			break;
		}
		_Cif_filetbl[cifd].form = NOT_A_CIF;
		if (fclose (_Cif_filetbl[cifd].fd)) return (CIF_SYSERR);

		if (_Cif_filetbl[cifd].ip != (char *) NULL)
		    (void) free(_Cif_filetbl[cifd].ip);

		/*
		 * If this is a temporary CIF created for cif_conv or
		 * cif_lines, then we should remove it
		 */

		if (_Cif_filetbl[cifd].tmp_cif)
		    (void) unlink(_Cif_filetbl[cifd].filename);

		if (_Cif_filetbl[cifd].filename != (char *) NULL) {
		    (void) free(_Cif_filetbl[cifd].filename);
		    _Cif_filetbl[cifd].filename = (char *) NULL;
		}
	}
	return (0);

}
