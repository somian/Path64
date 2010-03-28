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


#pragma ident "@(#) libu/ffio/sqbfcntl.c	92.1	06/29/99 13:16:47"


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ffio.h>
#include "sqbio.h"

/*
 * buffering layer fcntl requests
 *
 * Parameters:
 *	fd	- file descriptor (dummy)
 *	cmd	- command code
 *	arg	- command specific parameter
 *	iostat	- pointer to status return word
 *
 *	Returns: -1 if error occurred
 *		  0 if OK
 */
_sqb_fcntl(struct fdinfo *fio, int cmd, void *arg, struct ffsw *iostat)
{
struct fdinfo *llfio;
struct ffc_info_s *ffcp, locinfo;
struct sqb_f *sqb_info;
int ret, llfd;


	sqb_info = (struct sqb_f *) fio->lyr_info;
	llfio = fio->fioptr;
	ret = 0;

	switch(cmd)
		{
		case FC_GETINFO:
			ffcp = (struct ffc_info_s *)arg;
/*
 *			Only do a call to the lower level if it has been opened
 */
			if (llfio != NULL)
				{
				ret = XRCALL(llfio,fcntlrtn)
					llfio, FC_GETINFO, &locinfo, iostat);
				llfd = locinfo.ffc_fd; /* fd from lower layer */
				}
			else
				{
				locinfo.ffc_flags = 0;
				llfd = -1;
				}

			ffcp->ffc_flags = 
				FFC_STRM |	/* stream */
				FFC_WEOD |	/* can write EOD */

				FFC_RWND |	/* can rewind */

				FFC_BINARY |	/* can do binary */
				FFC_CODED |	/* can do chars */

				FFC_RDM |	/* can do random */
				FFC_SEQ |	/* can do seq */
				0;

			/* can do some operations only if lower layer can */
			ffcp->ffc_flags |= (locinfo.ffc_flags & 
				(FFC_NOTRN | 
				 FFC_SEEKA | 
				 FFC_SEEKR | 
				 FFC_SEEKE));

			ffcp->ffc_gran = 1;		/* 1 bit */
			ffcp->ffc_reclen = 0;	/* no rec length */
			ffcp->ffc_fd = llfd;
			break;
		case FC_STAT:
			ret = XRCALL(llfio,fcntlrtn)
					llfio, FC_STAT, arg, iostat);

			break;
#ifdef _CRAY
		case FC_GETTP:
			if (fio->rwflag == READIN || fio->rwflag == POSITIN) {
				/* synchronize physical position with */
				/* logical position */
				if (_sqb_sync(fio, iostat, 1) < 0) {
					return(ERR);
				}
			}
			else if (fio->rwflag == WRITIN) {
				if (_sqb_flush(fio, iostat) < 0) {
					return(ERR);
				}
			}
		/* fall through */
#endif
		case FC_SETRECL:
			ret = XRCALL(llfio,fcntlrtn)
					llfio, cmd, arg, iostat);
			break;
		case FC_ASPOLL:
		case FC_RECALL:
			/* all synch for now */
			break;
#ifdef _CRAY
		case FC_TSYNC:
		case FC_CLOSEV:
			if (fio->rwflag == WRITIN) {
				/* flush the last buffer and wait for writes */
				/* to complete */
				if (_sqb_flush(fio, iostat) < 0) {
					return(ERR);
				}
			}
			else {
				/* wait for asynchronous i/o to complete */
				if (_sqb_sync(fio, iostat, 0) < 0) {
					return(ERR);
				}
			}
			ret = XRCALL(llfio,fcntlrtn)
				llfio, cmd, arg, iostat);
			break;
#endif
		default:
			if (IS_RESERVED_FC(cmd)) {
				ret = XRCALL(llfio,fcntlrtn)
					llfio, cmd, arg, iostat);
			}
			else {
				ERETURN(iostat, FDC_ERR_NOSUP, 0)
			}
			break;
		}
	return(ret);
}
