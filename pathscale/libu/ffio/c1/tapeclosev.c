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


static char USMID[] = "@(#) libu/ffio/c1/tapeclosev.c	92.0	10/08/98 14:57:41";


#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include <fcntl.h>
#include <tapereq.h>
#include <sys/stat.h>
#include <sys/bmxctl.h>
#include <sys/dpacket.h>
#include <sys/types.h>
#include <sys/iosw.h>


/* 
 *	close volume - go to next tape
 *	works for either a bmx or er90 device
 */
int
_tape_closev(int fd)
{

	struct	dmn_comm	tapepos;

	tapepos.POS_REQ = TR_CLV;


	if(ioctl(fd, BXC_DMN_REQ, &tapepos) <0) {
		return(-1);
	}

	if( ioctl(fd, BXC_DMN_REP, &tapepos) <0) {
		return(-1);
	}
	if (tapepos.EOV_REP != 0) {
		errno = tapepos.EOV_REP;
		return(-1);
	}
	return(0);
}
