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


static char USMID[] = "@(#) libu/ffio/c1/tapesync.c	92.0	10/08/98 14:57:41";


#include  <sys/types.h>
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


/*
 *	_tape_sync - flush buffers out to tape
 *      Does not return until they are written.
 *	Returns: 0 if OK
 *           ENOSPC if EOV reached on sync
 *		     -1 if error (errno is set)
 */

_tape_sync(int fd)
{
	struct 	dmn_comm	bmxeov;

#if TRACE
	f_trace("flshbuf",0,0,0);
#endif
	bmxeov.EOV_REQ = TR_SYNC;
	bmxeov.EOV_REP = 0;
	bmxeov.EOV_STATUS = 0;

	if( ioctl(fd, BXC_DMN_REQ, &bmxeov) <0) {
#if TRACE
		f_trace("trsync",errno,0,0);
#endif
		return(-1);
	}

	if( ioctl(fd, BXC_DMN_REP, &bmxeov) <0) {
#if TRACE
		f_trace("flsictl",errno,0,0);
#endif
		return(-1);
	}
	if (bmxeov.EOV_REP != 0){
#if TRACE
		f_trace("flsrep",bmxeov.EOV_REP,0,0);
#endif
		if (bmxeov.EOV_REP == ENOSPC) {
			if (ioctl(fd,BXC_ACKERR,0) < 0) {
				return(-1);
			}
			return(ENOSPC);
		}
		else {
			errno = bmxeov.EOV_REP;
			return(-1);
		}
	}
	return(0);
}
