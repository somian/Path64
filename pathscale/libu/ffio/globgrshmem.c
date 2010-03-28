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


#pragma ident "@(#) libu/ffio/globgrshmem.c	92.3	10/14/99 15:22:06"

#include <fortran.h>
#include "globio.h"

extern void _shmem_group_inquire(int handle, shmem_group_t *sgs);

extern glio_group_t _glio_curgroup;

void
glio_group_shmem(int handle)
{

	glio_group_t *gg;
        shmem_group_t shg;

	gg = &_glio_curgroup;

	_shmem_group_inquire(handle, &shg);

	gg->groupsz = shg.groupsz;
	gg->myrank = shg.myrank;
	gg->grtype = GR_SHMEM;

	gg->u.shmem.group = handle;
}

void
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
glio_group_shmem_(
#else
GLIO_GROUP_SHMEM(
#endif
_f_int *handlep)
{
	glio_group_shmem(*handlep);
}
