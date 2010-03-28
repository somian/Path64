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


static char USMID[] = "@(#) libu/ffio/c1/bmxendsp.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <liberrno.h>
#include <ffio.h>
#include <fcntl.h>
#include <tapereq.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/bmxctl.h>
#include "bmxio.h"
#include "../sysio.h"

/*
 *	_bmx_endsp - end special processing
 *
 *	returns : 0 ok
 *		  -1 on error (errno is set)
 */


_bmx_endsp(BMXFIL *f)
{
	struct eovbuf *eovptr;
	struct eovbuf *oldptr;
	struct bmxio *ioptr;
	struct bmxio *saveptr;
	struct bmxio *ptr;
	int n, ret;


	ret = 0;
	/* Wait for i/o to be quiet */
	if(_bmx_quiet(f) != 0){
		return(-1);
	}	
	/* Flush library buffers */
	if( __bmxflush(f) != 0){	
		return(-1);
	}

	if(_end_eov(f->bmx_fd)!= 0)	/* Stop special processing */
		ret = -1;

	/* If EOV was reached while reading, goto clnup */
	if ((f->bmx_flag & BMXIO_WEOV) == 0)
		goto clnup;
	/* Go back to buffering writes */
	f->bmx_bufcnt = f->bmx_bufsz = f->bmx_bufszsv;

	if ((f->bmx_flag & BMXIO_MODELE) != 0){
/*		Do we have unread data in the 'fake buffer memory' list?
 *		If so, write it out. If part of the data has been read,
 *		then none will be written.
 */
		if ((f->eov_first == f->eov_current) 
			&& (f->bmx_buflstcnt == f->bmx_lstsz)) {

			/* Have unread data. Write it out. */
			eovptr = f->eov_current;
			while (eovptr != NULL){
				if (eovptr->bmx_bufmem != NULL){
					if (eovptr->bmx_buflst->state != 0) {
/*
 *						If we have unread data in the
 *						"fake" buffer memory, copy it
 *						back to the regular linked 
 *						list and write it.
 */
						memcpy(f->bmx_iofirst->bmx_list,
							eovptr->bmx_buflst,
						f->bmx_lstsz*sizeof(struct bmxlist));
						memcpy(f->bmx_iofirst->bmx_base,eovptr->bmx_bufmem,f->bmx_bufsz);
						n = _loop_write(f,f->bmx_iofirst,f->bmx_bufsz);
						if (n < 0){
							ret = -1;
						}
					}
				}
				eovptr = eovptr->eov_nxt;
			}
		}
		eovptr = f->eov_first;
		ioptr = f->bmx_iofirst;
		saveptr = ioptr;
		ioptr->bmx_nxt = f->bmx_lstcont;
		f->bmx_iofirst = f->bmx_iosavefirst;
		if (eovptr != NULL) {
			oldptr = eovptr;
			eovptr = eovptr->eov_nxt;
			free(oldptr->bmx_bufmem);
			free(oldptr->bmx_buflst);
			free(oldptr);
		}
	
		while (eovptr != NULL){
			ioptr = ioptr->bmx_nxt;
			if (ioptr == NULL)
				ioptr = f->bmx_iofirst;
			ioptr->bmx_base =  eovptr->bmx_bufmem;
			ioptr->bmx_list = eovptr->bmx_buflst;
			oldptr = eovptr;
			eovptr = eovptr->eov_nxt;
			free(oldptr);
		}
		f->eov_first = f->eov_current = NULL;

		f->bmx_buflstsz = 0;
		f->bmx_totblk = 0;
		f->bmx_bmblk = 0;
	}
	else {	
		/* MODEL D */
		/* Go back to multiple lists */
		ioptr = f->bmx_iofirst;
		saveptr = ioptr;
		ioptr->bmx_nxt = f->bmx_lstcont;
		f->bmx_iofirst = f->bmx_iosavefirst;
	}
	ptr = f->bmx_iofirst;
	/* Restore the bmx_prev pointer that was altered when EOV was reached */
	while ((ptr->bmx_nxt != saveptr) && (ptr->bmx_nxt != NULL)) {
		ptr = ptr->bmx_nxt;
	}
	saveptr->bmx_prev = ptr;
clnup:
	f->bmx_flag &= ~(BMXIO_EOVSEEN | BMXIO_WEOV);
	f->bmx_flag &= ~(BMXIO_SPPROC);
	f->bmx_iocurrent = f->bmx_iofirst;
	f->bmx_lstcont = NULL;

	_bmx_clear(f);		/* Clear all lists */
	f->bmx_bufptr = 0;	/* Reset bufptr */
	f->bmx_recl = 0;	/* clear record length */
	return(ret);
}
