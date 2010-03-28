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


#pragma ident "@(#) libu/ffio/trcpos.c	92.1	06/29/99 13:16:47"

#include <ffio.h>
#include "trcio.h"

/*
 *	trace positioning requests
 */

_trc_pos(fio, cmd, arg, len, stat)
struct fdinfo *fio;
int cmd;
long *arg;
int len;
struct ffsw *stat;
	{
	struct fdinfo *llfio;
	struct trace_f *trc_info;
	int ret;

	llfio = fio->fioptr;
	trc_info = (struct trace_f *)fio->lyr_info;

	_trc_enter(fio, TRC_POS);
	ret = XRCALL(llfio, posrtn) llfio, cmd, arg, len, stat);
	trc_info->lastseek = YES;
	_trc_exit(fio, ret, stat);
	}
