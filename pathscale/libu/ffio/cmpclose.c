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


#pragma ident "@(#) libu/ffio/cmpclose.c	92.2	06/29/99 13:16:47"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ffio.h>
#include "cmpio.h"
 
int
_cmp_close(fio, stat)
struct fdinfo *fio;
struct ffsw *stat;
{
	cmp_lyr	*cinfo;
        int    	fulp = FULL;
	int	ubc = 0, ret;
        struct  fdinfo *llfio;
	char	*rtnName = "_cmp_close";
 
        llfio = fio->fioptr;
	cinfo = fio->lyr_info;

	TRC_ENTER;

	/*
	 * If we are reading a compressed file we want to skip writing 
	 * anything to it.
	 */
	if ((cinfo->OpnFlags & O_WRONLY) == 0) {
		TRC_INFO("Data Cache size %d, hits %d, misses %d", 
			 cinfo->DSegCacheSize, cinfo->Hit, cinfo->Miss);
		goto close;
	}

	/*
	 * If 'cinfo->WroteHeader' is FALSE this means that user only
	 * did an ffopen() and an ffclose. In this case, we do not want
	 * to write anything to the file. Just proceed to close it.
	 */
	if (cinfo->WroteHeader == FALSE) {
		goto close;
	}

	/*
	 * Update the segment descriptor for the last data segment. 
	 * We know this by looking at the value of 'cinfo->bytesInBuf'. 
	 * If this is non-zero it means we have one last data segment 
	 * to compress and write to the file.
	 */
	if (cinfo->BytesInBuf) {
		if (_writeDSeg(llfio, stat, fulp, &ubc, cinfo) == ERR) {
			return(ERR);
		}
	}

	/*
	 * Now write the trailer for this compressed file.
	 */
	if (_writeTrailer(llfio, stat, fulp, &ubc, cinfo) == ERR) {
		return(ERR);
	}

close:
	/*
	 * Free the compress layer information structure.
	 */
	_free_cmp_lyr(cinfo);

	/*
	 * Close the trace file is one was opened.
	 */
	_CloseTraceFile(cinfo);

	/*
	 *  close file
	 */
        ret = XRCALL(llfio, closertn) llfio, stat);

	TRC_LEAVE;

        return(ret);
}
