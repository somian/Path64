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


#pragma ident "@(#) libu/ffio/cmpfcntl.c	92.2	06/29/99 13:16:47"

#include <ffio.h>
#include "cmpio.h"

/*
 * fcntl requests
 *
 * Parameters:
 *  fd      - fdinfo pointer
 *  cmd     - command code
 *  arg     - command specific parameter
 *  stat    - pointer to status return word
 *
 * This fcntl routine simply passes the request down to the next lower
 * layer, so it provides nothing of its own.
 *
 * When writing a user layer, the fcntl routine must be provided, and must
 * provide correct responses to one essential function and two desirable
 * functions.
 *
 *  FC_GETINFO: (essential)
 *  If the 'cmd' argument is FC_GETINFO, the fields of the 'arg' is
 *  considered a pointer to an ffc_info_s structure, and the fields must
 *  be filled. The most important of these is the ffc_flags field, whose
 *  bits are defined in <foreign.h>.(Look for FFC_STRM through FFC_NOTRN)
 *   FC_STAT: (desirable)
 *  FC_RECALL: (desirable)
 */
int
_cmp_fcntl(fio, cmd, arg, stat)
struct fdinfo *fio;
int cmd; 
void *arg;
struct ffsw *stat;
{
        int 	 ret = 0;
	cmp_lyr	*cinfo;
        struct   fdinfo *llfio;
	char	*rtnName = "_cmp_fcntl";
	struct   ffc_info_s *ffcp, locinfo;

        llfio = fio->fioptr;
	cinfo = (cmp_lyr *)fio->lyr_info;

	TRC_ENTER;

	TRC_LOG("Cmd = %d, arg = arg 0x%x", cmd, arg);

	switch (cmd) {
		case FC_GETINFO:
			ffcp = (struct ffc_info_s *) arg;
			ret = XRCALL(llfio,fcntlrtn)
				llfio, FC_GETINFO, &locinfo, stat);

			/* Flags are from lower layer */
			ffcp->ffc_flags = locinfo.ffc_flags;

			/*
			 * This layer makes it impposible to do LISTIO.
			 * In other words, a listio request will get
			 * broken into reada() and writea() requests.
			 */
			ffcp->ffc_flags &= ~(
				FFC_CANLISTIO | /* can NOT do listio */
				0);

			ffcp->ffc_fd = locinfo.ffc_fd; /* fd from lower layer */
			break;

		case FC_STAT:
                case FC_SETRECL:
                case FC_GETTP:
			/*
			 * Get what we can from the lower level.
			 */
			ret = XRCALL (llfio,fcntlrtn) llfio, cmd, arg, stat);
                        break;

		case FC_ASPOLL:
                case FC_RECALL:
                        break;

                default:
			if (IS_RESERVED_FC(cmd)) {
				ret = XRCALL(llfio,fcntlrtn)
					llfio, cmd, arg, stat);
			} else {
				ERETURN(stat, FDC_ERR_NOSUP, 0);
			}
			break;
	}


	TRC_LOG("Return %d", ret);

	TRC_LEAVE;

        return(ret);
}
