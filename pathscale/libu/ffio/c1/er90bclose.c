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


static char USMID[] = "@(#) libu/ffio/c1/er90bclose.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <ffio.h>
#include <stddef.h>
#include "er90by.h"

/*
 * ER90 byte stream close
 * _er90b_close() calls the system call close(2) and returns status
 */

_er90b_close(struct fdinfo *fio, struct ffsw *stat)
{
int ret;
ER90BYT *f;

	f = (ER90BYT *)fio->lyr_info;
	if (f->tpos) {
		if ( _tape_tpwait(f->fd, &(f->tpos)) != 0) {
			ERETURN(stat, errno, 0);
		}
	}
	ret = close(fio->realfd);
	if (f->tsireq.reply_fname != NULL) {
		ret |= unlink(f->tsireq.reply_fname);
		free(f->tsireq.reply_fname);
	}
	if (f->tsireq.request_fname != NULL) {
		free(f->tsireq.request_fname);
	}

	if (fio->lyr_info != NULL) {
		free(fio->lyr_info);
		fio->lyr_info = NULL;
	}

	if (ret != 0)
		ERETURN(stat, errno, 0);
	return (ret);
}
