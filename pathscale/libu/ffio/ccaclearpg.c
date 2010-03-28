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


#pragma ident "@(#) libu/ffio/ccaclearpg.c	92.1	06/29/99 13:16:47"


#include <stdio.h>
#include <ffio.h>
#if defined(BUILD_OS_DARWIN)
# include <limits.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <values.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <errno.h>
#include "ccaio.h"

/*
 * _cca_clear_page
 *
 *	De-assign a cache page from the current physical file page.
 *	If necessay, flush the page to disk first.
 *
 * Return value:
 *
 *	0	on success
 *	ERR	on error
 *
 */

_cca_clear_page(
struct cca_f	*cca_info,	/* cca_f structure for the file */
struct cca_buf	*cubuf,		/* the cache page to be cleared */
struct ffsw     *stat)		/* status return word */
{
  struct fdinfo   *llfio;        /* ffio file descriptor for underlying layer */
  int ret;
  off_t file_byte_pos;
  int j;
  struct ffsw err_stat;

    if( cca_info->file_number != cubuf->file_page.parts.file_number )
    {
      cca_info = _CCA_scache[cca_info->shared_cache].cca_info[cubuf->file_page.parts.file_number];
    }

    llfio    = cca_info->nextfio;

    if( cubuf->pending_asyncs )
    {
         struct cca_async_tracker *this_tracker;

         if(cubuf->sw.rw_mode == FFSW_READING)
         {
            CCAWAITIO(cubuf->sw,stat,ret);
            if( ret == ERR ) return(ERR);
         }

         this_tracker = cca_info->async_tracker;
         ret = 0;
         while( this_tracker )    /* check all trackers, may be more than one cache page involved to */
         {                        /* satisfy one incoming async request */
            if( this_tracker->cubuf == cubuf )
            {
               if( this_tracker->mode == CCA_TRACKER_READA || this_tracker->mode == CCA_TRACKER_WRITEA )
               {
                  int ret_value;
                  this_tracker->cubuf->pending_asyncs --;
                  _cca_complete_tracker( cca_info, this_tracker, &ret_value, 
			NULL, 0, 0, (bitptr)0, 0, NULL);
                  if (ret_value == ERR) {
			err_stat = *(this_tracker->stat);
			ret = ERR;
                  }
               }
            }
            this_tracker = this_tracker->next_tracker;
         }
         if (ret == ERR) 
         {
            *stat = err_stat;
            return(ERR);
         }
    }

    if( cubuf->sw.rw_mode == FFSW_READING ) /* Wait for async read to complete before proceeding */
    {
       CCAWAITIO(cubuf->sw,stat,ret);
       if( ret == ERR ) return(ERR);
    }

    if ( (cubuf->file_page.all != NULL_FILE_PAGE ) && ( cubuf->flags & CCA_DIRTY )  )  
    {
       if(cubuf->sw.llfio)
       {
          CCAWAITIO(cubuf->sw,stat,ret);  /* wait for current write before starting new write */
          if( ret == ERR ) return( ERR );
       }

       file_byte_pos = cubuf->file_page.parts.page_number * cca_info->byte_per_pg;
       ret = _cca_wrabuf(cca_info,
           llfio, cubuf,
	   cca_info->byte_per_pg,
           file_byte_pos,
           'a',	/* flush synchronously */
           stat);
	   if (ret == ERR) return(ERR);
    }

    cubuf->file_page.all = NULL_FILE_PAGE;  /* flag it as empty */

    for(j=0;j<cca_info->dirty_sectwds;j++) cubuf->unsynced_sectors[j] = 0;
    for(j=0;j<cca_info->dirty_sectwds;j++) cubuf->valid_sectors[j] = 0;
    for(j=0;j<cca_info->dirty_sectwds;j++) cubuf->sector_used[j] = 0;

    cubuf->adv_read      = FALSE;
    cubuf->chain_position = 0;
    cubuf->file_page.all = NULL_FILE_PAGE;

    return(0);
}
