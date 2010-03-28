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


static char USMID[] = "@(#) libu/ffio/c1/bmxswitchv.c	92.0	10/08/98 14:57:41";


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
#include "bmxio.h"

/*
 *	_bmx_closev - Switches tape volume
 *
 *	returns:
 *         0  OK
 *         -1 error (errno is set)
 */

_bmx_closev(BMXFIL *f)
{


	if (_bmx_prepos(f) != 0)
		return(-1);
	if( _tape_closev(f->bmx_fd) != 0)	/* Switch tape volume */
	{
		return(-1);
	}
	
	return(0);

}
