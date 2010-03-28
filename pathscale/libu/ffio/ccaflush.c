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


#pragma ident "@(#) libu/ffio/ccaflush.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ffio.h>
#include "ccaio.h"


/*
 * _cca_flush
 *
 *	Flush all dirty buffers to the underlying layer.
 *
 *	Return value:	 0 on success.
 *			-1 on failure.
 */
int
_cca_flush(
struct fdinfo	*fio,
struct ffsw	*stat
)
{
        int		i, nb, bs, ret;
	int		errv;
        struct cca_f	*cca_info;
	struct cca_buf	*cbufs;
        off_t           file_byte_pos;

        struct fdinfo   *llfio;

	errv	 = 0;
	cca_info = (struct cca_f *)fio->lyr_info;
	nb	 = cca_info->nbufs;
	bs	 = cca_info->bsize;

        llfio    = fio->fioptr;
	/*
	 * Loop once to start the flush of each dirty buffer.  Then loop a
	 * second time to wait for the completions of all buffer flushes.
	 */
           cbufs = cca_info->bufs;
	   for (i=0; i<nb; i++) 
           {
                if( cbufs[i].file_page.parts.file_number != cca_info->file_number ) continue;

                if( cca_info->optflags.scr == FALSE )
                {
                   if( cbufs[i].flags & CCA_DIRTY )  
                   {
                          if( cbufs[i].sw.llfio )
                          {
                             CCAWAITIO(cbufs[i].sw,stat,ret);
                             if (ret == ERR) errv = errv ? errv : stat->sw_error;
                          }

                          file_byte_pos = cbufs[i].file_page.parts.page_number *
					  cca_info->byte_per_pg;
                          ret = _cca_wrabuf(	cca_info,
                                        llfio,
					&cbufs[i],
                                        BITS2BYTES(bs),
					file_byte_pos,
					'a',		/*asynchronous*/
					stat);

                          if (ret == ERR) 
				errv = errv ? errv : stat->sw_error;
                   }
                }
	   }

	for (i=0; i<nb; i++)  /* wait for those still doing any I/O */
        {
          if(cbufs[i].sw.file_page.parts.file_number == cca_info->file_number )
          {
             CCAWAITIO(cbufs[i].sw,stat,ret);
             if (ret == ERR) errv = errv ? errv : stat->sw_error;
          }
	}

	if (errv) 
	{
		ERETURN(stat, errv, 0);
	}


        ret = XRCALL(llfio,flushrtn) llfio, stat);

        return( ret );
}
