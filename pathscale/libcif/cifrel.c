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


static char USMID[] = "@(#) libcif/cifrel.c	30.2	07/26/96 07:19:13";


/*
 * Cif_Release releases all space associated with a CIF file.  The "freemode"
 * argument indicates where space should be marked unused for later use
 * within the interface routines or returned to the heap via free().
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
#include <stdio.h>

#include "cif_int.h"

int Cif_Release
#ifdef __STDC__
(int cifd, int freemode)
#else
(cifd, freemode)
int cifd;				/* CIF descriptor */
int freemode;			/* mode of release */
#endif
{

	register int me;

	if (cifd < 0 || cifd >= CIF_FT_SIZE || _Cif_filetbl[cifd].form == NOT_A_CIF)
		return (CIF_NOTOPEN);
	else if (freemode != CIF_MEM_FREE && freemode != CIF_MEM_KEEP)
		return (CIF_BADREQ);

	if (_Cif_filetbl[cifd].mode == CIF_MEM_MANAGED) {

		/*
		 * Release all the memory entries and buffers after
		 * the first one. As we are not closing the file, there's
		 * no need to free the first one, we will be using it again
		 * very soon
		 */

		me = _Cif_filetbl[cifd].fme;
		me = _Cif_memarea[me].nme;
		while (me != 0) {

			if (freemode == CIF_MEM_FREE) {
				(void) free (_Cif_memarea[me].mbp);
				_Cif_memarea[me].mbp = NULL;
			}

			_Cif_memarea[me].used = 0;
			me = _Cif_memarea[me].nme;
		}

		/* Mark the first one empty */

		me = _Cif_filetbl[cifd].lme = _Cif_filetbl[cifd].fme;
		_Cif_memarea[me].nme = 0;
		_Cif_memarea[me].mused = 0;

	}
	return (0);
}
