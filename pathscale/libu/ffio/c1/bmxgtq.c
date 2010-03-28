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


static char USMID[] = "@(#) libu/ffio/c1/bmxgtq.c	92.0	10/08/98 14:57:41";


#include <liberrno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/bmxctl.h>
#include <sys/file.h>
#include <tapereq.h>
#include <errno.h>
#include "bmxio.h"

/*
 *	wait until outstanding asynchronous i/o is finished
 *
 *	Returns: 0 if everything is OK
 *		 1 if EOV was reached
 * 		 -1  if error (errno is set)
 */
#pragma soft _eov_wait
extern int _eov_wait(); 

_bmx_eovq(BMXFIL *f) 
{
 
	struct bmxio	*ioptr;
	struct bmxio	*oldioptr;
/*       
 *      Make sure that outstanding asynchronous i/o is complete
 */
	if ((f->bmx_flag & BMXIO_EOV) == 0) {
/*
 *		If we're not doing end of volume processing.
 */
		if (_bmx_quiet(f) != 0)
			return(-1);
		return(0);
	}
	else {
		if (f->bmx_flag & BMXIO_RW) {
			/* Handle EOV */
			if (f->bmx_flag & BMXIO_MODELE)
				ioptr = f->bmx_iocurrent;
			else
				ioptr = f->bmx_iocurrent->bmx_prev;
		
			oldioptr = ioptr;
			do {
				if(_eov_wait(f,ioptr) < 0)
					return(-1);
				if (f->bmx_flag & BMXIO_EOVSEEN) {
					return(1);
				}
				ioptr = ioptr->bmx_nxt;
				if (ioptr == NULL)
					ioptr = f->bmx_iofirst;
			} while (ioptr != oldioptr);
		}
		else {
			/* We were reading */
			/* Wait for all i/o to be quiet */
			/* If a read-ahead encountered EOV, just */
			/* remember that fact so that we can return */
			/* it later. */
			/* There is no data returned when we get ENOSPC */
			/* on a read. */
			ioptr = f->bmx_iocurrent;
			oldioptr = ioptr;
			do {
				if (ioptr->bmx_busy == ACTIVE) {
					WAITBMXIO(f,ioptr);
					if (BMXIO_ERROR(ioptr->bmx_iosw)) {
						(void)_bmx_clrerr(f);
						if ((ioptr->bmx_iosw.sw_error == ENOSPC)
						|| (ioptr->bmx_iosw.sw_error ==
						EBMXACKERR)) {
							;
						}
						else {
							errno = ioptr->bmx_iosw.sw_error;
							return(-1);
						}
					}
					else
						ioptr->bmx_busy = DATA;
				}
				ioptr = ioptr->bmx_nxt;
				if (ioptr == NULL)
					ioptr = f->bmx_iofirst;
			} while (ioptr != oldioptr);
		}
	}		
	return(0);
}
