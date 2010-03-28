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


static char USMID[] = "@(#) libu/ffio/c1/taperewd.c	92.0	10/08/98 14:57:41";

#include  <ffio.h>
#include  <sys/types.h>
#include  <sys/param.h>
#include  <sys/bmxctl.h>
#include  <sys/file.h>
#include  <sys/iosw.h>
#include  <sys/stat.h>
#include  <sys/jtab.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <tapereq.h>
#include  <errno.h>
#include  <liberrno.h>
#include  "tapeio.h"
 
/*
 * Issue a rewind request for a bmx or er90 device.
 */

extern int G@INTIO;

_tape_rewd(int fd)
{
	struct bmxpos	ctl;
	int ret;

	ctl.pos_fcn = TR_RWD;

	do {
		ret = ioctl(fd, BXC_SPOS, &ctl);
	} while ((ret < 0) && (G@INTIO == 0) && (errno == EINTR));

	return(ret);

}


/*
 * Wait for the positioning request to finish.
 */
_tape_tpwait(int fd, int *tpos)
{
	struct bmxpos	ctl;

waitresponse:
	if ( ioctl( fd, BXC_GPOS, &ctl) < 0 )
		return(-1);

	if ((ctl.pos_rc == EINTR) && (*tpos == TPOS_REWIND) &&
	 (G@INTIO == 0)) {
		if (_tape_rewd(fd) == -1)
			return(-1);
		goto waitresponse;		
	}
	if (ctl.pos_rc != 0){
		errno = ctl.pos_rc;
		return(-1);
	}
	*tpos = TPOS_DONE;
	return(0);

}
