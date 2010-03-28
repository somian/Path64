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


#pragma ident "@(#) libu/ffio/sqbclose.c	92.2	10/29/99 21:40:31"


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ffio.h>
#include "sqbio.h"

void _sqb_clfree(struct fdinfo *fio);

/*
 *	Close the buffering layer.
 *      Returns: -1 if error occurred
 *		  0 if OK
 */

_sqb_close(struct fdinfo *fio, struct ffsw *stat)
{
	struct fdinfo *llfio;
	int ret, ret2;
	
	llfio = fio->fioptr;

	ret = ret2 = 0;

/*
 *	Flush any buffers and wait for any asynchronous requests.
 */
	if (fio->rwflag == WRITIN){
		ret =_sqb_flush(fio, stat);
	}
	else if (fio->rwflag == READIN || fio->rwflag == POSITIN) {
		if (_sqb_sync(fio, stat, 0) < 0)
			ret = -1;
	}
	
/*
 * close the file
 */
	ret2 = XRCALL(llfio, closertn) llfio, stat);

/*
 * free the space
 */
	_sqb_clfree(fio);
	if ((ret | ret2) == 0) return(0);
	return(ERR);
}

/*
 * Free the space associated with the buffering layer.
 */
void
_sqb_clfree(struct fdinfo *fio)
{
	struct sqb_f *sqb_info;

	if (fio->fioptr != NULL) {
		free(fio->fioptr);
		fio->fioptr = NULL;
	}
	sqb_info = (struct sqb_f *)fio->lyr_info;
	if (sqb_info != NULL) {
		if (sqb_info->sqb_buf != 0) {
			free(BPTR2CP(sqb_info->sqb_buf));
		}
		if (sqb_info->sqbio_base != NULL) {
			free(sqb_info->sqbio_base);
		}
		free(sqb_info);
		fio->lyr_info = NULL;
	}
	return;
}
