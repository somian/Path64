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


#pragma ident "@(#) libu/ffio/f77close.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ffio.h>
#include "fxlist.h"
#include "f77io.h"


/*
 * Close an f77 file.
 */
int
_f77_close(struct fdinfo *fio, struct ffsw *stat)
{
        struct fdinfo *llfio;
        int ret;
	struct f77_xf *xfinfo;

        ret = 0;

/*
 *      If the file has been writing, flush and truncate after the last
 *      record written.
 */
	xfinfo = (struct f77_xf *)fio->lyr_info;
        if (fio->rwflag == WRITIN) {

                ret = XRCALL(fio, flushrtn) fio, stat);
                if (ret != 0) return(ERR);

                ret = XRCALL(fio, weodrtn) fio, stat);
                if (ret != 0) return(ERR);
        }

        if (xfinfo->_base != NULL)
                free(xfinfo->_base);      /* free buffer */
        if (fio->lyr_info != NULL)
                free((char *)fio->lyr_info);    /* free private storage */
        llfio = fio->fioptr;
        if (llfio != NULL) {
                ret = XRCALL(llfio, closertn) llfio, stat);
                free(llfio);            /* free info block */
        }
        return(ret);
}

