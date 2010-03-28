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


static char USMID[] = "@(#) libu/ffio/c1/bmxgabs.c	92.0	10/08/98 14:57:41";


#include  <sys/types.h>
#include  <sys/tpdctl.h>
#include  <sys/bmxctl.h>
#include  <sys/file.h>
#include  <sys/iosw.h>
#include  <sys/stat.h>
#include  <sys/jtab.h>
#include  <sys/param.h>
#include  <fcntl.h>
#include  <tapereq.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <errno.h>
#include  <liberrno.h>
#include  <ffio.h>
#include  <stdarg.h>
#include  "bmxio.h"
#include  "../sysio.h"

/*
 *	Get absolute tape position
 *	Returns: 0 if OK. blockid contains the position
 *		-1 if error (errno is set)
 *		
 */

_bmx_gabs(BMXFIL *f, void *blockid)
{

	if ( f->bmx_tpos ) {
		if ( _tape_tpwait( f->bmx_fd, &(f->bmx_tpos) ) != 0)
			return(-1); 
	}

/*
 *      If previous i/o request was a write request make sure the list
 *      is flushed before getting the position.
 */
 
        if ( f->bmx_flag & BMXIO_RW ) {
		if (f->bmx_flag & BMXIO_EOV == 0) {
			/* Not doing EOV */
			if (f->bmx_lstptr->state == 0 &&
				f->bmx_lstptr->bytes != 0) {
				/* error if we just wrote part of a record */
				errno = FDC_ERR_NOTREC;
				return(-1);
			}
                	else  {
				if (__bmxflush(f) < 0)
					return(-1);
			}
			/* A sync is REQUIRED on Model D IOS systems. */
			/* Do it for all systems. */
			if (_tape_sync(f->bmx_fd) < 0)
				return(-1);
		}
		else {
			/* Flushing out the data in the library buffers, or doing
			 * the sync could cause us to hit EOV. Call _bmx_gtpos because
			 * it knows what to do about EOV.
			 */
			if (_bmx_gtpos(f, NULL, 0, 1) != 0)
				return(-1);
		}
	}
	else {
		/* wait for i/o to be quiet */
		if (_bmx_eovq(f) < 0)
			return(-1);
	}
	return(ioctl(f->bmx_fd, TPC_GABS, blockid));

}
/*
 * Set absolute tape position.
 * Returns: 0 all ok
 *	    -1 if error (errno set)
 */
_bmx_sabs(BMXFIL *f, ...)
{
	struct tpdpos posreq;
	long blockid;
	va_list args;

	if (_bmx_prepos(f) != 0)
		return(-1);

	va_start(args, f);
	blockid = va_arg(args, long);
	va_end(args);

	posreq.pos_fcn = TR_PABS;
	posreq.pos_vsn = (long) blockid;
	posreq.pos_count = 0;
	posreq.pos_rc = 0;
	if (ioctl(f->bmx_fd, TPC_DMN_REQ, &posreq) < 0) {
		return(-1);
	}
	if (ioctl(f->bmx_fd, TPC_DMN_REP, &posreq) < 0) {
		return(-1);
	}
	if (posreq.pos_rc != 0) {
		errno = posreq.pos_rc;
		return(-1);
	}
	f->bmx_flag &= ~(BMXIO_EOVSEEN);
	f->bmx_flag &= ~(BMXIO_TPMK);
	return(0);
}

