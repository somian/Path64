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


#pragma ident "@(#) libu/ffio/ccawritea.c	92.2	10/11/99 15:29:41"


#include <stdio.h>
#include <errno.h>
#include <ffio.h>
#include "ccaio.h"

/*
 * _cca_writea
 *
 * Process writea requests for the cachea layer.
 *
 * Return value:
 *
 *	The number of bytes to be transferred is returned upon successful 
 *	completion.
 *
 *	If an error occurs, -1 is returned.  
 *
 *	The stat structure is not set until asynchronous I/O completion,
 *	unless an immediately detected error is encountered.   
 */
_cca_writea(
struct fdinfo	*fio, 		/* ffio file descriptor. */
bitptr		datptr,		/* bit pointer to the user's data. */
int		nbytes,		/* Nuber of bytes to be written. */
struct ffsw	*stat,		/* pointer to status return word */
int		fulp,		/* full or partial write mode flag */
int		*ubcp		/* pointer to unused bit count.  On return, */
				/* *ubcp is updated to contain the unused bit */
				/* count in the data returned. */ 
)
{
	int		cpos;		/* bit position in file */
	int		moved;		/* number of bits transfered */
	int		bytes_moved;	/* number of bytes transfered */
	int		morebits;	/* bits moved in current iteration */
	int		pgoff;
	int		fileaddr;
	int		gb_rd;		/* nonzero if pages must be read */
	int		i, nbits, bs, nbu;
	int		olpos, endpos, endoff;
	bitptr		toptr;
	struct ffsw	locstat;
	struct fdinfo	*llfio;
	struct cca_f	*cca_info;
	struct cca_buf	*cubuf;
        int             j;
        FILE_PAGE       file_page;
        struct cca_async_tracker *this_tracker;
        int completed;
        struct ffsw	err_stat;	    /* status return word */
        int ret;
        int err;

	CLRFFSTAT(locstat);

	cca_info = (struct cca_f *)fio->lyr_info;
	llfio	 = fio->fioptr;

        if(CCA_SOFT_BYPASS) {
             int ret;
             ret = XRCALL(llfio,writertn) llfio, datptr, nbytes, stat, fulp, ubcp );
             return( ret );
        }

        if( cca_info->optflags.no_write ) {
	    errno = EBADF;
	    ERETURN(stat, EBADF, 0);
        }
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
        /* Although this layer is capable of handling non-zero ubc */
        /* and bitptrs that aren't on a byte boundary, we are not */
        /* supporting this right now on mips systems. */
        if (*ubcp != 0){
                err = FDC_ERR_UBC;
                goto err1_ret;
        }
        if ((BPBITOFF(datptr) & 07) != 0) {
                err = FDC_ERR_UBC;
                goto err1_ret;
        }
#endif

	nbits = BYTES2BITS(nbytes) - *ubcp;

	fio->rwflag = WRITIN;

	if (nbits == 0) {			/* quick return for nbits == 0*/
		SETSTAT(stat, FFCNT, 0);
		return(0);
	}

/*
 *	Initialize user's async status structure.
 */
	stat->sw_flag	= 0;
	stat->sw_count	= 0;
	stat->sw_stat	= 0;
	stat->sw_error	= 0;

/*
 *	Move data from user to buffer 
 */
	bs	 = cca_info->bsize;	/* bit size of each buffer */
	cpos	 = cca_info->cpos;	/* current file position */
	olpos    = cpos;		/* save original position */
	fileaddr = CCAFLOOR(cpos,bs);	/* bit offset within the file of the cache page */
        completed = TRUE;

	while (nbits > 0) {
		/*
		 * Find the cache buffer assigned to the current page.  If
		 * no buffer is currently assigned, then _cca_getblk assigns 
		 * one.
		 */
		pgoff	  = cpos - fileaddr;	/* offset within the page */

                file_page.parts.page_number = fileaddr/bs;
                file_page.parts.file_number = cca_info->file_number;

		CCA_FINDBLK(cca_info, file_page, cubuf, err_stat, ret);
		if (ret == ERR) {
			goto err_ret;
		}

		if (cubuf == NULL) {	/* if data not buffer-resident*/

                        cca_info->write_miss ++;
			endpos = cpos + nbits; /*1 bit past the end*/
			endoff = endpos - CCAFLOOR(endpos,bs);

			gb_rd = (pgoff || endoff);
			cubuf = _cca_getblk_p(cca_info, llfio, fileaddr, gb_rd,
					      &err_stat, NULL, 'w' , 'a' );
                        if( cubuf == NULL )
			{
				goto err_ret;
			}
		}
		else {
                        cca_info->write_hit ++;
		}

		morebits  = MIN(nbits, bs - pgoff);

                CHRONOMETER(cubuf,"writing"); /* adjust last access time*/

                this_tracker = _cca_start_tracker( llfio,
						   cca_info,
						   cubuf,
						   stat,
						   CCA_TRACKER_WRITEA,
						   datptr,
						   pgoff,
						   morebits );

                if( this_tracker == (struct cca_async_tracker *)(ERR) ) {
                   err_stat = *stat;
                   goto err_ret;
                }
                else if( this_tracker )
                   completed = FALSE;

		SET_BPTR(datptr, INC_BPTR(datptr, morebits));

		cpos  += morebits;
	        cca_info->cpos   = cpos;
	        if (cpos > cca_info->fsize) cca_info->fsize = cpos;
		nbits -= morebits;
		fileaddr = cpos;        /* cpos is page-aligned after 1st pass*/
	}
	moved		 = cpos - olpos;
	fio->recbits	+= moved;

	bytes_moved = BITS2BYTES(moved);
        if( completed )
        {
           stat->sw_flag = 1;
           stat->sw_stat = 0;
        }

	return(bytes_moved);
err_ret:
	/*
	 * If outstanding asynchronous requests connected to this one 
	 * have been started, wait for them.
	 */
	if (nbits != BYTES2BITS(nbytes) - *ubcp) {
		(void)_cca_fcntl(fio, FC_RECALL, stat, &locstat);
	}
	*stat = err_stat;
	stat->sw_flag = 1;
	errno = stat->sw_error;
	return(ERR);

err1_ret:
	ERETURN(stat, err, 0);
}
