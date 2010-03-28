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


static char USMID[] = "@(#) libu/ffio/c1/er90bfcntl.c	92.0	10/08/98 14:57:41";


#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <errno.h>
#include <ffio.h>
#include "er90by.h"

/*
 * ER90 byte stream fcntl requests
 *
 * Parameters:
 *	fio	- fdinfo pointer
 *	cmd	- command code
 *	arg	- command specific parameter
 *	iostat	- pointer to status return word
 */

_er90b_fcntl(struct fdinfo *fio, int cmd, int arg, struct ffsw *iostat)
{
int ret;
struct ffsw *uiostat;
struct ffc_info_s *ffcp;
struct stat *stp;
struct ffc_gettp_s *gtptr;

	ret = 0;
	switch(cmd){
		case FC_GETINFO:
			ffcp = (struct ffc_info_s *)arg;
			ffcp->ffc_flags = 
				FFC_STRM |	/* stream */
				FFC_WEOD | 	/* can WEOD (trunc()) */

				FFC_RWND |	/* can rewind */

				FFC_BINARY |	/* can do binary */
				FFC_CODED |	/* can do chars */

				FFC_SEQ |	/* can do seq */
				FFC_ASYNC |	/* can do 'real' async I/O */

				FFC_NOTRN |	/* No data transformation */
				0;

			ffcp->ffc_gran = 8;	/* granularity is 1 byte */
			ffcp->ffc_reclen = 0;	/* no rec length */
			ffcp->ffc_fd = fio->realfd; /* real fd */
			break;
		case FC_STAT:
			stp = (struct stat *)arg;
			ret = fstat(fio->realfd, stp);
			if (ret < 0)
				iostat->sw_error = errno;
			if (stp->st_oblksize == 0)
				stp->st_oblksize = 2002944;	/* bytes */
			if (stp->st_blksize == 0)
				stp->st_blksize = 4096;	/* bytes */
			break;
		case FC_RECALL:
			uiostat = (struct ffsw *) arg;
			if (uiostat->sw_flag == 0) {
				recall(fio->realfd, 1, (struct iosw **)&uiostat);
			}
			/* fall through */
		case FC_ASPOLL:
			uiostat = (struct ffsw *) arg;
			if (uiostat->sw_flag == 0)
				break;
/*
 *			If status is non-zero, it is already set.  Leave it
 *			alone.
 */
			if (FFSTAT(*uiostat) == 0) {
				if (uiostat->sw_error != 0) {
					FFSTAT(*uiostat) = FFERR;
				}
				else if (uiostat->sw_count == 0) {
					SETSTAT(uiostat, FFEOD, 0);
				}
				else {
					FFSTAT(*uiostat) = FFCNT;
				}
			}
			break;
		case FC_CLOSEV:
			if ( _tape_closev(((ER90BYT *)fio->lyr_info)->fd) != 0)
				ERETURN(iostat, errno, 0);
			break;
		case FC_GETTP:
			/* get tape info */
			/* Arg is a pointer to structure gettp. */
			gtptr = (struct ffc_gettp_s *)arg;
			if (_er90b_gettp(fio, 
				gtptr->ffc_pa, gtptr->ffc_glen,
				gtptr->ffc_synch) != 0)
				ERETURN(iostat, errno, 0);
			break;
               case FC_TSYNC:
                        if (_er90b_gettp(fio, NULL, 0, 1) != 0)
                                ERETURN(iostat, errno, 0);
                        break;
		default:
			ERETURN(iostat, FDC_ERR_NOSUP, 0);
	}
	return(ret);
}
