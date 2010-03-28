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


#pragma ident "@(#) libu/ffio/cosfcntl.c	92.2	10/07/99 22:14:49"

#include <ffio.h>
#include "cosio.h"

/*
 * cos fcntl requests
 *
 * Parameters:
 *	fd	- file descriptor (dummy)
 *	cmd	- command code
 *	arg	- command specific parameter
 *	stat	- pointer to status return word
 */
int
_cos_fcntl(
struct fdinfo *fio,
int cmd, 
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
int arg,
#else
void *arg,
#endif
struct ffsw *stat)
	{
	struct fdinfo *llfio;
	struct ffc_info_s *ffcp, locinfo;
	int ret;
	struct cos_f *cos_info;

	llfio = fio->fioptr;

	ret = 0;
	switch(cmd)
		{
		case FC_GETINFO:
			ffcp = (struct ffc_info_s *)arg;
			ret = XRCALL(llfio,fcntlrtn)
				llfio, FC_GETINFO, &locinfo, stat);
			ffcp->ffc_flags = 
				FFC_REC |	/* records */
				FFC_WEOF |	/* can weof */
				FFC_WEOD |	/* can weod */

				FFC_BKSP |	/* can backspace */
				FFC_RWND |	/* can rewind */

				FFC_VAR |	/* can do variable len recs */
				FFC_BINARY |	/* can do binary */
				FFC_CODED |	/* can do chars */

				FFC_SEQ |	/* can do seq */
				FFC_WRTRUNC |	/* Write implies trunc */
				0;
			
			/* We can seek absolute (getpos trick) and seek end */
			/* only if the lower levels can */
			ffcp->ffc_flags |= locinfo.ffc_flags & FFC_SEEKA;
			ffcp->ffc_flags |= locinfo.ffc_flags & FFC_SEEKE;
			ffcp->ffc_gran = 1;	/* granularity is 1 bit */
			ffcp->ffc_reclen = 0;	/* no rec length */
			ffcp->ffc_fd = locinfo.ffc_fd; /* fd from lower layer */
			break;
		case FC_GETTP:
                        cos_info = (struct cos_f *)fio->lyr_info;
                        ASWAITUP(fio, stat, &cos_info->bstat, cos_info, ret);
		case FC_STAT:
		case FC_SETRECL:
			ret = XRCALL(llfio,fcntlrtn) llfio, cmd, arg, stat);
			break;
		case FC_ASPOLL:
		case FC_RECALL:
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
