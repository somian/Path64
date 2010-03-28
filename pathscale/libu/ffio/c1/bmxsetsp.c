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


static char USMID[] = "@(#) libu/ffio/c1/bmxsetsp.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include <fcntl.h>
#include <tapereq.h>
#include <sys/stat.h>
#include <sys/bmxctl.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include "bmxio.h"

/*
 *	bmxsetsp - Initializes/terminates special EOV processing
 *
 *	f - BMXFIL pointer
 *
 *	iflag
 *	  0 - Turn BOV/EOV processing OFF
 *	  nonzero - Turn BOV/EOV processing ON
 *
 *	Returns:
 *	  0 - OK
 *        -1 - error
 */


_bmx_setsp(BMXFIL *f, int iflag)
{

	/* If we just did a rewind, wait for it to complete */
	if ( f->bmx_tpos ) {
		if ( _tape_tpwait( f->bmx_fd, &(f->bmx_tpos)) != 0 ) {
			return(-1);
		}
	}
	/*
	 * Check to make sure i/o is quiet before changing
	 * EOV processing.
	 */

	if(_bmx_quiet(f)!=0) {
		return(-1);
	}

	if(__bmxflush(f)!= 0) {
		return(-1);
	}
	if (iflag == 0) {
		/* Turn EOV processing OFF */

		if( _disable_eov(f->bmx_fd) != 0) {
			return(-1);
		}
		f->bmx_flag &= ~(BMXIO_EOV | BMXIO_EOVSEEN | BMXIO_WEOV | BMXIO_SPPROC);
	}
	else {
		/* Turn EOV processing ON */

		if(_enable_eov(f->bmx_fd) != 0) {
			return(-1);
		}
		f->bmx_flag |= BMXIO_EOV;
		f->bmx_flag &= ~(BMXIO_EOVSEEN | BMXIO_WEOV | BMXIO_SPPROC);
	}
	return(0);
}
