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


#pragma ident "@(#) libu/ffio/cdcfcntl.c	92.1	06/29/99 13:16:47"

#include <ffio.h>
#include "cdcio.h"

/*
 * CDC class fcntl requests
 *
 * Parameters:
 *	fd	- file descriptor (dummy)
 *	cmd	- command code
 *	arg	- command specific parameter
 *	stat	- pointer to status return word
 */
_cdc_fcntl(fio, cmd, arg, stat)
struct fdinfo *fio;
int cmd, arg;
struct ffsw *stat;
	{
	struct fdinfo *llfio;
	struct ffc_info_s *ffcp, locinfo;
	int ret, flags;

	llfio = fio->fioptr;

	ret = 0;
	switch(cmd)
		{
		case FC_GETINFO:
			ffcp = (struct ffc_info_s *)arg;
			ret = XRCALL(llfio,fcntlrtn)
				llfio, FC_GETINFO, &locinfo, stat);
			flags = 
				FFC_REC |	/* records */
				FFC_WEOD |	/* can weod */

				FFC_RWND |	/* can rewind */

				FFC_VAR |	/* can do variable len recs */

				FFC_SEQ |	/* can do sequential I/O */
				FFC_WRTRUNC |	/* Write implies trunc */
				0;
			switch(fio->rtype)
				{
				case TR_CDC_CZ:
					flags |= FFC_CODED;
/*
 *					CZ supports EOF only on lower layer.
 */
					if (locinfo.ffc_flags & FFC_WEOF)
						flags |= FFC_WEOF;
					/* granularity is 6 bits, one char */
					ffcp->ffc_gran = 6;
					break;
				case TR_CDC_CS:
					flags |= FFC_WEOF;
					flags |= FFC_BINARY;
					/* granularity is 60 bits, one word */
					ffcp->ffc_gran = 60;
					break;
				case TR_CDC_IW:
					flags |= FFC_WEOF;
					flags |= FFC_BINARY;
					flags |= FFC_CODED;
					/* granularity is 1 bit */
					ffcp->ffc_gran = 1;
					break;
				case TR_CDC_CW:
					flags |= FFC_WEOF;
					flags |= FFC_BINARY;
					flags |= FFC_CODED;
					/* granularity is 1 bit */
					ffcp->ffc_gran = 1;
					break;
				}

			ffcp->ffc_flags = flags;

			ffcp->ffc_reclen = 0;	/* no rec length */
			ffcp->ffc_fd = locinfo.ffc_fd; /* fd from lower layer */
			break;
		case FC_STAT:
		case FC_SETRECL:
		case FC_GETTP:
			ret = XRCALL(llfio,fcntlrtn) llfio, cmd, arg, stat);
			break;

		case FC_ASPOLL:
		case FC_RECALL:
			/* all synchronous */
			break;
                default:
			if (IS_RESERVED_FC(cmd))
				{
				ret = XRCALL(llfio,fcntlrtn)
					llfio, cmd, arg, stat);
	                        }
			else
				{
				ERETURN(stat, FDC_ERR_NOSUP, 0)
				}
			break;
		}
	return(ret);
	}
