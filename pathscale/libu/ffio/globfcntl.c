/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libu/ffio/globfcntl.c	92.2	06/29/99 13:16:47"


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ffio.h>
#include "globio.h"

/*
 * global layer fcntl requests
 *
 *	Returns: -1 if error occurred
 *		  0 if OK
 */
_glob_fcntl(
struct fdinfo	*fio,		/* file descriptor */
int		cmd,		/* command code */
void		*arg,		/* command-specific parameter */
struct ffsw	*iostat)	/* pointer to status return word */
{
        struct glob_f *glob_info = (struct glob_f *)fio->lyr_info;
	struct fdinfo *llfio;
	struct ffc_info_s *ffcp, locinfo;
	struct ffc_mtinfo_s *mti;
	int ret, llfd;
	int *lyrinfo;
	int remote_pe, remote_index;
	struct par_file_data *fdptr = &glob_info->fstate;
	int fd;

        struct ffsw *check_ffsw;
        struct cca_async_tracker *this_tracker;

	llfio = fio->fioptr;

	ret = 0;

	switch(cmd) {

	case FC_GETINFO:
		ffcp = (struct ffc_info_s *)arg;
/*
 *		Only do a call to the lower level if it has been opened
 */
		if (llfio != NULL) {
			ret = XRCALL(llfio,fcntlrtn)
					llfio, FC_GETINFO, &locinfo, iostat);
			llfd = locinfo.ffc_fd; /* fd from lower layer */
		}
		else {
			locinfo.ffc_flags = 0;
			llfd = -1;
		}

		ffcp->ffc_flags = 
				FFC_STRM |	/* stream */
				FFC_WEOD |	/* can write EOD */

				FFC_SEEKA |	/* seek abs */
				FFC_SEEKR |	/* seek relative */
				FFC_SEEKE |	/* seek end */
				FFC_RWND |	/* can rewind */

				FFC_BINARY |	/* can do binary */
				FFC_CODED |	/* can do chars */

				FFC_RDM |	/* can do random */
				FFC_SEQ |	/* can do seq */
				FFC_NOCLOSE |	/* Don't try to close on abt. */
				0;

		/* no data transformation */

		ffcp->ffc_flags |= locinfo.ffc_flags & FFC_NOTRN;

		ffcp->ffc_gran = 8;	/* 8 bits */
		ffcp->ffc_reclen = 0;	/* no rec length */
		ffcp->ffc_fd = llfd;
		break;

	case FC_STAT:
/*
 *		Fill in most of the fields from the lower layer.  
 */
		ret = XRCALL(llfio,fcntlrtn) llfio, FC_STAT, arg, iostat);

/*
 *		No need to lock the fdata structure here because the
 *		size field will always be updated atomically (by a 
 *		store or a PUT.
 */
                _glio_shmem_getmem(glob_info->arp, fdptr, fdptr,
			sizeof(*fdptr), glob_info->fstate_owner);

/*
 *		The size returned reflects the global layer's idea of the 
 *		file size.
 */
#ifdef KEY /* Bug 1678 */
		((struct stat *)arg)->st_size = fdptr->size;
#else /* KEY Bug 1678 */
		((struct ffc_stat_s *)arg)->st_size = fdptr->size;
#endif /* KEY Bug 1678 */
		break;

	case FC_ASPOLL:
		break;

	case FC_RECALL:
		break;

	case FC_SCRATCH:
		{
		int sflgs;
		ret = XRCALL(llfio,fcntlrtn) llfio, cmd, &sflgs, iostat);
		*(int*)arg = sflgs;
		break;	
		}

	default:
		if (IS_RESERVED_FC(cmd)) {
			ret = XRCALL(llfio,fcntlrtn) llfio, cmd, arg, iostat);
		}
		else {
		     	ERETURN(iostat, FDC_ERR_NOSUP, 0)
		}
		break;

	}
	return(ret);
}
