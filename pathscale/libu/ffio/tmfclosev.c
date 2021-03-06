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


#pragma ident "@(#) libu/ffio/tmfclosev.c	92.1	06/29/99 13:16:47"

#include "tmfio.h"
#include <stdlib.h>
#include <sys/types.h>

/*
 *	_tmf_closev - Switches tape volume
 *
 *	returns:
 *		 0	OK
 *		-1	error (errno is set)
 */
int
_tmf_closev(struct fdinfo *fio, struct ffsw *st)
{
	tmfreqhdr_t	ctl;
	struct tmfio	*xfinfo;

	xfinfo	= (struct tmfio *)fio->lyr_info;

	if (xfinfo->tmf_tpos){
		if (_tmf_tpwait(xfinfo) < 0){
			return(ERR);
		}
	}
/*
 *	If the file's been writing, flush out data.
 *	If we've been reading, clear out any unread data.
 */
	if (_tmf_flush(fio, st) < 0){
		return(ERR);
	}

	bzero(&ctl, sizeof(tmfreqhdr_t));
	ctl.request	= TR_CLV;

	if(ioctl(xfinfo->tmf_fd, TMFC_DMNREQ, &ctl) <0) {
		ERETURN(st, errno, 0);
	}

	if (ctl.reply != 0) {
		ERETURN(st, ctl.reply, 0);
	}

	return(0);
}
