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


#pragma ident "@(#) libu/waio/c1/closewa.c	92.1	08/19/99 14:46:33"

#include  <errno.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/listio.h>
#include  <malloc.h>
#include  <ffio.h>
#include  "../waio.h"
 
/*
 *	closewa - close word addressable dataset   
 */
CLOSEWA(index)
long *index;
{
	WAFIL *f;
	struct fdinfo *fio;
 
	f = wafils + (*index-1);		/* pointer to WAFIL table */
	fio = GETIOB(f->wa_fd);

	if (f->wa_buffer != (char *)0)
		{
	  	(void)free(f->wa_buffer);	/* free the buffer space */
		f->wa_buffer = 0;
		}

	if (f->wa_fdc)
		{
		if (XRCALL(fio, closertn) fio, &f->wa_iosw) != 0)
			_lerror(_LELVL_ABORT, f->wa_iosw.sw_error);
		}
	else if (close(f->wa_fd) < 0)
		_lerror(_LELVL_ABORT, errno);
	f->wa_fd = 0;  
	if (f->wa_up != NULL)
		_clruptr(f->wa_up);
	return(0);
 
}
