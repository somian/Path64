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


static char USMID[] = "@(#) libu/ffio/c1/bmxstartsp.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <liberrno.h>
#include <ffio.h>
#include <fcntl.h>
#include <tapereq.h>
#include <sys/stat.h>
#include <sys/bmxctl.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include "bmxio.h"

/*
 *	_bmx_startsp - Begins special processing at EOV
 *
 *
 *	returns: 
 *	  0   OK
 *	  -1 - Not OK (errno set)
 */


_bmx_startsp(BMXFIL *f)
{
	long pa[16];


	if(_bmx_quiet(f)!=0){	/* Wait for i/o to be quiet */
		return(-1);
	}

/*
 * 	During special processing, we want to write one block at 
 * 	a time. This simplifies the daemon's and driver's life. 
 * 	To force this, set our buffer count = mbs. When special 
 * 	processing ends, we will go back to buffering writes.
 */

	f->bmx_bufcnt = f->bmx_bufsz = rtoc(f->bmx_mbs);

	if (f->bmx_flag & BMXIO_MODELE){
/*
 * 	If MODELE IOS, we need to know how many blocks are in
 * 	system memory. If the user wants to read these from buffer
 * 	memory, we need to know when to switch and give her the
 * 	data buffered by the library.
 */
		if (_bmx_gtpos(f,pa,15,0) != 0) {
			return(-1);
		}
		f->bmx_bmblk = pa[11];
		f->bmx_totblk = f->bmx_bmblk;	/* 0 blocks on tape */
	}
	if( _start_eov(f->bmx_fd) != 0)	/* Start special EOV processing */
	{
		return(-1);
	}
	f->bmx_flag |= BMXIO_SPPROC;
	f->bmx_flag &= ~BMXIO_EOVSEEN;
	_bmx_clear(f);
	/* Reset bufptr so that we can do reads from buffer memory */
        f->bmx_bufptr =0;
	f->bmx_recl = 0;	/* clear record length */
	return(0);

}
