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


#pragma ident "@(#) libu/ffio/ccasyncpage.c	92.1	06/29/99 13:16:47"

#include <stdio.h>
#include <errno.h>
#include <ffio.h>
#include "ccaio.h"

/*
 * _cca_sync_page
 *
 *	Flush a cache page to the file.
 */
_cca_sync_page(
struct cca_f	*cca_info,
struct cca_buf	*cubuf,
struct ffsw     *stat		/* pointer to status return word */
)

{
   int			i;
   int          	ret;
   int          	partial_dirty;
   off_t          	file_byte_pos;
   struct fdinfo	*llfio;

   file_byte_pos = cubuf->file_page.parts.page_number * cca_info->byte_per_pg;
   llfio = cca_info->nextfio;

   if(cubuf->sw.rw_mode == FFSW_WRITING ) {
      CCAWAITIO(cubuf->sw,stat,ret);
      if(ret==ERR)return(ERR);
   }

   partial_dirty = FALSE;
   for(i=0;i<cca_info->dirty_sectwds;i++) {
      if( cubuf->unsynced_sectors[i] != cca_info->dirty_sectors_check[i] ) {
         partial_dirty = TRUE;
         break;
      }
   }

/*
 * If the page is partially valid, call _cca_wrabuf to flush the dirty
 * valid sectors and then call _cca_rdabuf to read back the entire page.
 *
 * (Why don't we just pre-read the invalid sectors and defer the syncing??
 *  this is done for writes, so the page is about to become dirty again...)
 */
   if( partial_dirty ) {
      if( cubuf->sw.llfio ) {
         CCAWAITIO(cubuf->sw,stat,ret);
         if(ret==ERR) return(ERR);
      }

      ret = _cca_wrabuf(cca_info,
             llfio, cubuf,
             cca_info->byte_per_pg ,
             file_byte_pos ,
             'a',
             stat);
       if( ret == ERR ) return( ERR );

       if( cubuf->sw.llfio ) {
          CCAWAITIO(cubuf->sw,stat,ret);
          if(ret==ERR) return(ERR);
       }

       ret = _cca_rdabuf(cca_info, 
               llfio, cubuf, 
               cca_info->byte_per_pg ,
               file_byte_pos ,
               's' , 
               stat);
       if( ret == ERR ) return( ERR );

       cubuf->pre_init = TRUE;
       return(1);
   }
   else {
      cubuf->pre_init = TRUE;
      return(0);
   }
}
