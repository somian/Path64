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


#pragma ident "@(#) libu/ffio/sysclose.c	92.1	06/29/99 13:16:47"

#include <errno.h>
#include <ffio.h>
#include <stdlib.h>
#include "sysio.h"

/*
 * _sys_close() calls the system call close(2) and returns status
 */

_sys_close(struct fdinfo *fio, struct ffsw *stat)
{
	int ret;
	struct sys_f	*sys_info;

	ret = close(fio->realfd);
	if (ret < 0)
		ERETURN(stat, errno, 0);

	sys_info = (struct sys_f *)fio->lyr_info;
	if (sys_info != NULL) {
		if (sys_info->name != NULL)
			free(sys_info->name);	/* free the saved file name */
		free(sys_info);
	}

	return (ret);
}
