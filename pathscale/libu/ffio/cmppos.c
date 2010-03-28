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


#pragma ident "@(#) libu/ffio/cmppos.c	92.2	06/29/99 13:16:47"

#include <ffio.h>
#include "cmpio.h"
 
/*
 *  trace positioning requests
 */
 
int
_cmp_pos(fio, cmd, arg, stat)
struct fdinfo *fio;
int cmd;
long *arg;
struct ffsw *stat;
{
   	int 	 ret;
	long	 bytes;
	cmp_lyr	*cinfo;
	char	*rtnName = "_cmp_pos";
 
	cinfo = fio->lyr_info;

	TRC_ENTER;
 
	switch (cmd) {
#ifndef __mips
		case FP_BSEEK:
		    /*
		     * Before supporting this on MIPS, we need to look at
		     * what the arguments should be. A long (for *arg) is
		     * not big enough to hold the total file offset on 
		     * 32-bit machines.
		     */
		    if ((*arg & 7) != 0) {
			    bytes = (-(*arg))>>3;
			    bytes = -bytes;
		    } else {
			    bytes = *arg>>3;
		    }

		    ret = XRCALL(fio, seekrtn) fio, bytes, *(arg+1), stat);

		    return(_dshiftl(ret, ret, 3));
#endif
		default:
		    ERETURN(stat, FDC_ERR_NOSUP, 0);
	}
}
