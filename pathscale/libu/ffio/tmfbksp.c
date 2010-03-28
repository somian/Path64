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


#pragma ident "@(#) libu/ffio/tmfbksp.c	92.1	06/29/99 13:16:47"
 
#include "tmfio.h"

/*
 * backspace a tape file
 * Parameters:
 *	fd	- file descriptor (dummy)
 *	stat	- pointer to status return word
 *
 * Returns:
 *	0 on success 
 *	-1 on error with sw_error containing error code
 *	The sw_stat field is set only on error. Note that
 *	layers are not consistent about setting this after
 *	positioning. COS layer will set sw_stat to FFEOF if
 *	we just backspaced over an end of file. bmx layer doesn't
 *	do anything with it.
 */

int
_tmf_bksp(struct fdinfo *fio, struct ffsw *stat)
{
	struct tmfio	*xf_info;
	register int	ret;
	int		usertm;

	xf_info	= (struct tmfio *)fio->lyr_info;

	if (xf_info->tmf_tpos) {
		if (_tmf_tpwait (xf_info) < 0) {
			ERETURN(stat, errno, 0);
		}
	}
/*
 *	If the file's been writing, flush out any unwritten data.
 */
	if (xf_info->rwflag == WRITIN) {
		if (_tmf_flush(fio, stat) < 0) {
			return(ERR);
		}
	}
/*
 *	Now backspace.
 */	
	ret	= _tmf_stpos(fio, FP_TPOS_BACK, 1, 0, 0, 0, &usertm, stat);

	if (ret < 0 && stat->sw_error != ETBOF)
		return(ERR);

	xf_info->rwflag	= POSITIN;

	return(0);
}
