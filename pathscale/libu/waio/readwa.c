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


#pragma ident "@(#) libu/waio/readwa.c	92.1	06/23/99 13:55:03"

#include "waio.h"
#include <sys/file.h>
#include <sys/listio.h>
#include <errno.h>

extern int G@INTIO;	/* declared in libu/ffio */

/*
 *	readwa - read word addressable dataset 
 */

READWA(index,buffer,addr,count,ifasync)
long *index, *buffer, *addr, *count, *ifasync;
{

	WAFIL *f;
	struct fflistreq *lp;
	long bn;
	int ret;
	int rtype;
 
	f = wafils + (*index-1);
	bn = (long)(*addr-1) >> L2NWPBLK;
/*
 *	If i/o is still busy, go into recall until the request
 *	has been completed.
 */
	WAITWAIO(f);

/*
 *	Check for errors on previous asynchronous request.
 */
	if ( WAIO_ERROR(f) )
		_errwa("READWA", "System I/O error on", f, f->wa_iosw.sw_error);
#if DEBUG
	if (FDCTRACE(TRACE_WA)) {
		printf(" position before read, offset = %d\n", bn*NBPBLK);
		printf(" read, number of bytes = %d\n",(unsigned)(*count*NBPW));
	}
#endif
/*
 *	Setup for listio write request.
 *	Most fields are preset in OPENWA, so do not have to be set here.
 */
	lp = &f->wa_list;
	lp->li_opcode = LO_READ;
	lp->li_buf = (char *) buffer;
	lp->li_offset = bn*NBPBLK;
	lp->li_nbyte = (long)(*count*NBPW);
/*
 *	If async has not been requested, use the listio function with wait i/o
 *	set.  Otherwise, start the i/o and immediately return to the caller.
 *	If async I/O, zero out the last block of the buffer to ensure that
 *	a final partial block is padded with 0's instead of garbage.
 */
	if (*ifasync == 0)
		rtype = LC_WAIT;	/* synchronous read */
	else {
		rtype = LC_START;	/* asynchronous read */
		memset((char*)buffer + lp->li_nbyte - NBPBLK, 0, NBPBLK);
	}

restart:
/*
 *	Clear asynch status word for next request.
 *	Note that FFSTAT is cleared *only* if FFIO.
 */
	f->wa_iosw.sw_flag  = 0;
	f->wa_iosw.sw_error = 0;
	f->wa_iosw.sw_count = 0;

	if (f->wa_fdc == YES) {
		FFSTAT(f->wa_iosw) = 0;		/* Don't trust anyone! */
		ret = fflistio(rtype, lp, 1);
	}
	else
		ret = listio(rtype, (struct listreq *)lp, 1);

	if (ret != 1) {
		if ( errno == EINTR && G@INTIO == 0 )
			goto restart;
		_errwa("READWA", "List I/O error on", f, errno);
	}
/*
 *	If request was synchronous, or is done, check for errors.
 */
	if ( !WAIO_BUSY(f) ) {
		int bytes_read;
/*
 *		Check for errors on read request.
 */
		if ( WAIO_ERROR(f) ) {
			if (f->wa_iosw.sw_error == EINTR && G@INTIO == 0)
				goto restart;
			_errwa("READWA", "System I/O error on", f,
				f->wa_iosw.sw_error);
		}
/*
 *		If no errors, then the count better be nonzero.  Otherwise,
 *		our end-of-data (EOI) pointer is wrong.
 */
		bytes_read = f->wa_iosw.sw_count;
		if (bytes_read == 0)
			_errwa_abort(FEINTUNK);
		else if (bytes_read < (long)(*count*NBPW)) {
			memset((char*)buffer+bytes_read, 0,
				lp->li_nbyte - bytes_read);
		}
	}
	return(0);
}
