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


static char USMID[] = "@(#) libu/ffio/c1/bmxquiet.c	92.0	10/08/98 14:57:41";


#include <liberrno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/bmxctl.h>
#include <sys/file.h>
#include <tapereq.h>
#include <errno.h>
#include "bmxio.h"
#include "../sysio.h"

/*
 *	wait until all outstanding asynchronous i/o is finished
 *
 *	Returns: 0 if everything is OK
 * 		 -1 if error (errno is set)
 */
 
_bmx_quiet(BMXFIL *f) 
{
 
	struct bmxio	*ioptr;
	int lstcnt;
	int ret;
/*       
 *      Make sure that outstanding asynchronous i/o is complete
 */
	ret = 0;
	ioptr = f->bmx_iofirst;
	lstcnt = f->bmx_lstsz;
	while (ioptr != NULL){
		if (ioptr->bmx_busy == ACTIVE){
			WAITBMXIO(f,ioptr);
			if (BMXIO_ERROR(ioptr->bmx_iosw)){
				if (ioptr->bmx_iosw.sw_error != EBMXACKERR){
					errno = ioptr->bmx_iosw.sw_error;
					ret = -1;
				}
			}
			if (f->bmx_flag & BMXIO_RW){
				ioptr->bmx_busy = INACTIVE;	/* If writing */
				memset(ioptr->bmx_list,0,sizeof(struct bmxlist) * lstcnt);
			}
			else if (ret != -1)
				ioptr->bmx_busy = DATA;	/* If reading */
		}	
		ioptr = ioptr->bmx_nxt;
	}
	return(ret);
}
_bmx_clrerr(BMXFIL *f)
{
	/* Issue an ioctl to clear the error. */
	if (ioctl(f->bmx_fd,BXC_ACKERR,0) < 0){
		return(-1);
	}
	return(0);
}

/*
 *	Clear all lists
 *
 */

void
_bmx_clear(BMXFIL *f)
{
int lstcnt;
struct bmxio *ioptr;
	ioptr = f->bmx_iofirst;
        lstcnt = f->bmx_lstsz;
	while (ioptr !=NULL){
		ioptr->bmx_busy = INACTIVE;
		ioptr->bmx_iosw.sw_flag = 0;
		memset(ioptr->bmx_list,0,sizeof(struct bmxlist) * lstcnt);
		ioptr = ioptr->bmx_nxt;
	}
	f->bmx_iocurrent = f->bmx_iofirst;
	f->bmx_lstptr = f->bmx_iocurrent->bmx_list;
	f->bmx_lstcnt = f->bmx_lstsz;
	f->bmx_bufcnt = f->bmx_bufsz;	/* buffer is empty */
	f->bmx_bufptr = f->bmx_iocurrent->bmx_base;	/* buffer is empty */
}

/*
 * If previous i/o request was a write request make sure the list
 * is flushed
 */
__bmxflush(BMXFIL *f)
{
struct bmxio *ioptr;
int n;
	if (f->bmx_flag & BMXIO_RW) {
		if (f->bmx_lstcnt != f->bmx_lstsz) {
			ioptr = f->bmx_iocurrent;
			ioptr->bmx_iosw.sw_count = 0;
			ioptr->bmx_iosw.sw_error = 0;
			ioptr->bmx_iosw.sw_flag = 0;
			n = _loop_write(f, ioptr, f->bmx_bufsz);
			if (n < 0) {
				return(-1);
			}
			ioptr->bmx_busy = INACTIVE;
			memset(ioptr->bmx_list,0,sizeof(struct bmxlist) * f->bmx_lstsz);
                        ioptr = ioptr->bmx_nxt;
                        if (ioptr == NULL)
                                f->bmx_iocurrent = f->bmx_iofirst;
                        else
                                f->bmx_iocurrent = ioptr;
			f->bmx_bufptr = f->bmx_iocurrent->bmx_base;
			f->bmx_lstptr = f->bmx_iocurrent->bmx_list;
			f->bmx_lstcnt = f->bmx_lstsz;
		}
	}
	return(0);
}
/*
 * _loop_write calls the write() system call, and if it is interrupted,
 * it re-writes any data that was not transferred
 *
 * Returns 0 on success
 *        <0 on failure
 */
_loop_write(BMXFIL *f, struct bmxio *ioptr, int bs)
{
	int n;
	int lstcnt;
	int cl;
	struct bmxlist *lstptr;
	char *bufptr;

	f->bmx_listptr = ioptr->bmx_list;
	n = write(f->bmx_fd, ioptr->bmx_base, bs);
	if (n < 0) {
		if (errno == EINTR) {
			/* Some of the data may have been written */
			/* go through the list until we find unwritten data */
			lstcnt = f->bmx_lstsz;
			lstptr = ioptr->bmx_list;
			bufptr = ioptr->bmx_base;
			while ((lstptr->flags & (BMF_BTR | BMF_WTM)) && lstcnt) {
				cl = rtoc(lstptr->bytes);
				bufptr += cl;
				lstptr++;
				lstcnt--;
			}
			if ((lstptr->bytes != 0) || (lstptr->state != 0)) {
				memmove(ioptr->bmx_base, bufptr, f->bmx_bufsz -
				(bufptr - ioptr->bmx_base));
				memmove(ioptr->bmx_list,lstptr,
				lstcnt*sizeof(struct bmxlist));
				memset((ioptr->bmx_list + lstcnt),0,(f->bmx_lstsz - lstcnt) *sizeof(struct bmxlist));
				n = _loop_write(f, ioptr, bs);
			}
			else {
				/* Must have written all of it */
				n = 0;
			}
		}
	}
	return(0);
}
