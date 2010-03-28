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


static char USMID[] = "@(#) libu/ffio/c1/er90brewind.c	92.0	10/08/98 14:57:41";

#include <errno.h>
#include "er90by.h"

/*
 *	rewind ER90 byte-stream file
 *	Returns: 0 if everything is OK
 *		-1 if error (errno is set)
 */

_er90b_rewd(f) 
ER90BYT 	*f;
{
 

	if ( f->tpos ) { 
		if( _tape_tpwait( f->fd, &(f->tpos) ) != 0)
			return(-1);
	}

/*
 * 	Issue rewind request
 */ 
	if ( _tape_rewd( f->fd ) == -1){
		return(-1);
	}
	f->tpos = TPOS_REWIND;
	return(0);

}

