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


#pragma ident "@(#) libu/ffio/sqbseek.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <ffio.h>
#include "sqbio.h"

/*
 *
 * Perform a seek operation on a buffered file.  
 *
 * Parameters:
 *	fio - fdinfo pointer
 *	pos - requested byte offset
 *	whence - SEEK_SET, SEEK_CUR, or SEEK_END
 *	stat - status return word
 *
 * Return value:
 *
 *	The new byte postion in the file.
 *	If an error occurs, -1 is returned, and the stat->sw_error field is 
 *	set.
 */
_ffseek_t
_sqb_seek(struct fdinfo *fio, off_t pos, int whence, struct ffsw *stat)
{
	_ffseek_t	ret;
	struct sqb_f	*sqb_info;
	int sync = 0;
	struct fdinfo *llfio;

	sqb_info = (struct sqb_f *)fio->lyr_info;
	llfio = fio->fioptr;

	/* We could probably do more record keeping, and */
	/* determine whether the desired position was already */
	/* in our buffers. */

	/* Wait for outstanding I/O */
        if (fio->rwflag == READIN || fio->rwflag == POSITIN) {
		if (whence == SEEK_CUR)
			sync = 1;	/* synchronize physical and logical */
					/* positions */
                if (_sqb_sync(fio, stat, sync) < 0) {
                        return(ERR);
                }
        }
        else if (fio->rwflag == WRITIN) {
                if (_sqb_flush(fio, stat) < 0) {
                        return(ERR);
                }
        }
	fio->rwflag = POSITIN;
	ret = XRCALL(llfio, seekrtn) llfio, pos, whence, stat);
	return(ret);	
}
