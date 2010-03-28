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


static char USMID[] = "@(#) libu/ffio/c1/bmxrewind.c	92.0	10/08/98 14:57:41";


#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/iosw.h>
#include <sys/bmxctl.h>
#include <sys/file.h>
#include <tapereq.h>
#include <errno.h>
#include "bmxio.h"

/*
 *	rewind on-line tape file
 *	Returns: 0 if everything is OK
 *		-1 if error (errno is set)
 */

_bmx_rewd(BMXFIL *f) 
{
 

	if (_bmx_prepos(f) != 0)
		return(-1);
/*
 * 	Issue rewind request
 */ 
	if ( _tape_rewd( f->bmx_fd ) == -1){
		return(-1);
	}
	f->bmx_tpos = TPOS_REWIND;
	f->bmx_flag &= ~(BMXIO_EOVSEEN);
	f->bmx_flag &= ~(BMXIO_TPMK);
	return(0);

}
/*
 * 	Do necessary stuff before we issue a positioning request
 *	Returns: 0 if everything is OK
 *		-1 if error (errno is set)
 */
_bmx_prepos(BMXFIL *f)
{
	if ( f->bmx_tpos ) { 
		if( _tape_tpwait( f->bmx_fd, &(f->bmx_tpos) ) != 0)
			return(-1);
	}
/*
 *	Make sure that active i/o is quiet before doing rewind 
 */ 
	if( _bmx_quiet(f) < 0){
		/* Ignore read-ahead errors */
		if (f->bmx_flag & BMXIO_RW)
			return(-1);	
	}
/*	
 *	If previous i/o request was a write request make sure the list
 *	is flushed before issuing the rewind.  
 */
	if (__bmxflush(f) < 0){
		return(-1);
	}

/*
 * 	Clear any errors - for example, read ahead may have 
 * 	read a tapemark
 */
	if (_bmx_clrerr(f) < 0)
		return(-1);
	_bmx_clear(f);		/* Clear all lists */
	f->bmx_bufptr  = 0;	/* indicate empty buffer */
	f->bmx_recl   = 0;	/* and clear record length */
	return(0);
}
