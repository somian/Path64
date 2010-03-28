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


#pragma ident "@(#) libu/ffio/cosclose.c	92.1	06/29/99 13:16:47"

#include <ffio.h>
#include <stdlib.h>
#include "cosio.h"



/*
 *	Close a COS blocked sequential file
 *      Returns: -1 if error occurred
 *		  0 if OK
 */

int
_cos_close(struct fdinfo *fio, struct ffsw *stat)
	{
	struct fdinfo *llfio;
	int ret;
	struct cos_f *cos_info;

	cos_info = (struct cos_f *)fio->lyr_info;
	llfio = fio->fioptr;

	cos_info->cos_flag |= COS_IOCLOSE;	
	if (cos_info->cos_flag & COS_IOWRITE) {	/* if just did a write	*/
		/*  ensure proper EOD	*/
		if (XRCALL(fio, weodrtn) fio, stat) < 0)
			goto badret;
	}

	if(cos_info->cos_flag & COS_IOWRT) {
		if(cos_info->cos_flag & COS_IODIRTY) {	/* flush buffer if dirty */
			if (_cos_iflush(fio, stat) < 0)
				goto badret;
		}
	}
/*
 *	We have recorded the location of the bit after the last EOD
 *	written.  Truncate the file.  The user
 *	may have written that EOD, then rewound and read.  No matter.
 *	We truncate by virtue of having written that EOD.  This means that
 *	truncation does not conflict with things like pre-allocation.
 */
	if (cos_info->cos_lasteod > 0) 
		{
		int lasteodb;	/* last eod byte */

		lasteodb = CEIL(cos_info->cos_lasteod, BLKSZ) >> 3;
		if (XRCALL(llfio, seekrtn) llfio, lasteodb, 0, stat) >= 0)
			{
			cos_info->cos_diskpos = lasteodb;
			if (XRCALL(llfio, weodrtn) llfio, stat) < 0)
				goto badret;
			}
		}

/*
 *	Make sure that any outstanding I/O is complete.  The close does
 *	*not* clear it.
 */
	ASWAIT(fio, &cos_info->bstat);

	ret = XRCALL(llfio, closertn) llfio, stat);	/* close file */

	_cos_clfree(fio);
	return(ret);

badret:
	XRCALL(llfio, closertn) llfio, stat);	/* be sure t'close it */
	_cos_clfree(fio);
	return(ERR);
	}

void
_cos_clfree(struct fdinfo *fio)
	{
	struct cos_f *cos_info;

	cos_info = (struct cos_f *)fio->lyr_info;
	if (cos_info != NULL)
		{
		if (BPTR2CP(cos_info->obuf) != NULL)
			{
			free(BPTR2CP(cos_info->obuf));
			SET_BPTR(cos_info->obuf, CPTR2BP(NULL));
			}
		}
	if (BPTR2CP(fio->_base) != NULL)
		{
		free(BPTR2CP(fio->_base));
		SET_BPTR(fio->_base, CPTR2BP(NULL));
		}
	if (fio->lyr_info != NULL)
		{
		free(fio->lyr_info);
		fio->lyr_info = NULL;
		}
	if (fio->fioptr != NULL)
		{
		free(fio->fioptr);
		fio->fioptr = NULL;
		}
	return;
	}
