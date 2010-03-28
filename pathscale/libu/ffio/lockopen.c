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


#pragma ident "@(#) libu/ffio/lockopen.c	92.3	10/29/99 21:40:31"


#include <ffio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "fxlist.h"
#include "locklyr.h"

DECLARE(LOCK_XLIST);
struct xtr_s LOCK_XLIST_P = { LOCK_XLIST };

/*
 * Open the lock layer. This open routine is called directly from _ffopen.
 * It is not called with an XRCALL. The user cannot specify the lock layer.
 */

struct fdinfo *
_open_lock_lyr (struct fdinfo *fio, plock_t *lockptr)
{
	struct fdinfo *nfio;
	extern struct xtr_s *_recfm_tab[NUM_CLASSES];

	nfio = (struct fdinfo *)calloc(1,sizeof(*fio));
	if (nfio == NULL) 
		{
		errno = FDC_ERR_NOMEM;
		return(NULL);
		}
	memset(nfio, 0, sizeof(*fio));
	nfio->magic 	= MAGIC_ID;
	nfio->class 	= CLASS_LOCK;
	nfio->opn_flags	= fio->opn_flags;
	nfio->xr	= *_recfm_tab[CLASS_LOCK];
		
	/* Leave the nfio->lock field = 0. This layer does not need to have */
	/* a locking layer on top of it. */
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	nfio->can_listio = 1;	/* the lock layer can listio */
#endif
	nfio->fioptr = fio;
	nfio->lyr_info = (void *)lockptr;
	/* Do not set up fio->parptr yet. That is done in the calling routine */
	return(nfio);
}
