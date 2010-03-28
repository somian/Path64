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


#pragma ident "@(#) libu/ffio/tmfclose.c	92.1	06/29/99 13:16:47"


#include "tmfio.h"
#include "fxlist.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

/*
 * Close a tape file.
 */
int
_tmf_close(struct fdinfo *fio, struct ffsw *stat)
{
	register int	ret;
	struct tmfio	*xfinfo;

	ret	= 0;
	SETSTAT(stat, 0, 0);
/*
 *	If the user closes while in eov processing, buffered memory is lost.
 */
	xfinfo	= (struct tmfio *)fio->lyr_info;

	if (xfinfo->tmf_tpos) {
		if (_tmf_tpwait (xfinfo) < 0) {
			ERETURN(stat, errno, 0);
		}
	}
/*
 *	If the file has been writing, flush and truncate after the last
 *	record written.
 */
	if (xfinfo->rwflag == WRITIN) {

		ret	= XRCALL(fio, flushrtn) fio, stat);
		if (ret != 0) return(ERR);

		ret	= XRCALL(fio, weodrtn) fio, stat);
		if (ret != 0) return(ERR);
	}

	ret	= close(xfinfo->tmf_fd);

	if (ret != 0) {
		SETSTAT(stat, errno, 0);
		return(ERR);
	}

	if (xfinfo->tmf_base != NULL)
		free(xfinfo->tmf_base);	/* free buffer */

	if (xfinfo->eovbase != NULL)
		free(xfinfo->eovbase);	/* free buffer */

	if (fio->lyr_info != NULL)
		free((char *)fio->lyr_info);	/* free private storage */

	return(0);
}
