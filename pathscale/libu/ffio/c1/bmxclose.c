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


static char USMID[] = "@(#) libu/ffio/c1/bmxclose.c	92.0	10/08/98 14:57:41";


#include <sys/param.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/bmxctl.h>
#include <sys/file.h>
#include <tapereq.h>
#include <errno.h>
#include <ffio.h>
#include "bmxio.h"

/*
 *	close on-line tape file
 *
 */
 

_bmx_close(fio, stat) 
struct fdinfo *fio;
struct ffsw *stat;
{
 
	register int n = 0;
	BMXFIL *f;

	f = (BMXFIL *)fio->lyr_info;

	if ( f->bmx_tpos ) {	/* Wait for positioning to finish */
		if( _tape_tpwait( f->bmx_fd, &(f->bmx_tpos) ) != 0)
			ERETURN(stat, errno, 0);
	} 	
	
/*
 *	Make sure i/o is quiet before closing.
 */

	if ( _bmx_quiet(f) < 0) {
		/* Ignore read-ahead errors */
		if (f->bmx_flag & BMXIO_RW)
			n = -1;
	}
/*
 *      If previous i/o request was a write request make sure the list
 *      is flushed before issuing the close.  
 */ 
	n |= __bmxflush(f);

	if (f->bmxio_alloc != NULL) {
		free(f->bmxio_alloc->bmx_base);
		free(f->bmxio_alloc->bmx_list);
		free(f->bmxio_alloc);
	}
	/* Have we made a reply pipe? If so, remove it */
	if (f->tsireq.reply_fname != NULL){
		n |= unlink(f->tsireq.reply_fname);
		free(f->tsireq.reply_fname);
	}
	if (f->tsireq.request_fname != NULL){
		free(f->tsireq.request_fname);
	} 
	n |= close( f->bmx_fd );
	if ( n < 0 )
		ERETURN(stat, errno, 0);
	free((char *)f);

	return(0);

}
