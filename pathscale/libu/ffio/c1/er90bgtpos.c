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


static char USMID[] = "@(#) libu/ffio/c1/er90bgtpos.c	92.0	10/08/98 14:57:41";


#include  <sys/types.h>
#include  <sys/file.h>
#include  <sys/param.h>
#include  <fcntl.h>
#include  <tapereq.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <errno.h>
#include  <ffio.h>
#include  "er90by.h"

/*
 *	Get tape position for ER90 byte stream
 *	Returns: 0 if OK
 *		-1 if error (errno is set)
 */

_er90b_gettp(struct fdinfo *fio, long *pa, long palen, long synch)
{
	ER90BYT *f;
	struct tsdata	tsdata;
	int 		vsn[MAXVSN];

	f = (ER90BYT *)fio->lyr_info;
	if ( f->tpos ) {
		if( _tape_tpwait( f->fd, &(f->tpos) ) != 0)
			return(-1); 
	}

	if (synch == 1 && fio->rwflag == WRITIN) {
		if(_tape_sync(f->fd) != 0)
			return(-1);
	}

	if( _tape_tptsi (&(f->tsireq), &tsdata, vsn) != 0)
		return(-1);
	_tape_gtpos(&tsdata, vsn, 0, pa, palen);
	return(0);
}
