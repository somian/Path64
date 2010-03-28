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


#pragma ident "@(#) libu/ffio/tmfseek.c	92.1	06/29/99 13:16:47"


#include "tmfio.h"
#include <stdlib.h>
#include <sys/types.h>

/*
 * The only type of "seek" we allow for a tape file is rewind.
 */
_ffseek_t
_tmf_seek(
	struct fdinfo	*fio,
	off_t		pos,
	int		whence,
	struct ffsw	*stat	
)
{
	tmfreqhdr_t	ctl;
	struct tmfio	*xfinfo;

	xfinfo	= (struct tmfio *)fio->lyr_info;

	if (xfinfo->tmf_tpos) {
		if (_tmf_tpwait(xfinfo) < 0) {
			ERETURN(stat, errno, 0);
		}
	}
/*
 *	If the file's been writing, flush out data.
 *	If we've been reading, clear out any unread data.
 */
	if (_tmf_flush(fio, stat) < 0) {
		return(ERR);
	}

	switch (whence) {
		case SEEK_SET:
			
			if (pos != 0) {
				ERETURN(stat, FDC_ERR_NOSUP, 0);
			}
			else {
				/* rewind */
				bzero(&ctl, sizeof(ctl));
				ctl.request	= TR_RWD;
				/* Set async. This means that this */
				/* call may return before the tape is rewound */
				ctl.async	= 1;
				if (ioctl(xfinfo->tmf_fd, TMFC_DMNREQ, &ctl) < 0) {
					ERETURN(stat, errno, 0);
				}
				/* The next time we do something to this tape */
				/* we need to verify that the rewind finished */
				xfinfo->tmf_tpos	= TPOS_REWD;
			}
			break;

		default:
			ERETURN(stat, FDC_ERR_NOSUP, 0);
	}

	xfinfo->rwflag	= POSITIN;

	return(0);	/* we can't return a byte position */
}
