/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


static char USMID[] = "@(#) libcif/cifmode.c	30.2	07/26/96 07:19:13";


/* -------------------------------------------------------------------------
 * Cif_Memmode sets the memmory management mode for a CIF file.  This return
 * must be called after Cif_Open but before any calls to Cif_Getrecord to
 * be effective.  The mode may be one of three values:
 *
 *   CIF_MEM_INDIV   = Each record structure is returned as an individually
 *                     malloc'd block
 *   CIF_MEM_FIXED   = Each record structure is returned using the same
 *                     internally malloc'd space
 *   CIF_MEM_MANAGED = Space for each record is allocated from larger
 *                     internally malloc'd buffers.  The entire amount of
 *                     space can be free'd at one time.
 * -------------------------------------------------------------------------
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

int Cif_Memmode
#ifdef __STDC___
(int cifd, int mode)
#else
(cifd, mode)
int cifd;			/* cif descriptor of file */
int mode;			/* memory management mode selector */
#endif
{

	int me;

	if (cifd < 0 || cifd >= CIF_FT_SIZE || _Cif_filetbl[cifd].form == NOT_A_CIF)
		return (CIF_NOTOPEN);
	else if (mode < 1 || mode >= CIF_MEM_MAX ||
	         _Cif_filetbl[cifd].mode != CIF_MEM_DEFAULT ||
				_Cif_filetbl[cifd].optype != 'r')
	{
		return (CIF_BADREQ);
	}

	if (mode != CIF_MEM_INDIV) {

		/* If memarea table not present, get it. */

		if (_Cif_memasize == 0) {
			if ((me =_Cif_memtbl()) != 0)
				return (me);
		}

		/* Get the first memory entry and buffer for the file */

		if ((me = _Cif_mementry (CIF_BUFSIZE)) < 0)
			return (me);
		_Cif_filetbl[cifd].fme = _Cif_filetbl[cifd].lme = me;
	}
	_Cif_filetbl[cifd].mode = (short) mode;
	return (0);

}
