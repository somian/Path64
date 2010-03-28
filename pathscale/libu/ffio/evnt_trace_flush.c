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


#pragma ident "@(#) libu/ffio/evnt_trace_flush.c	92.2	10/07/99 22:15:19"


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ffio.h>
#include "evntio.h"

/*
 * _evnt_trace_flush
 *
 * Write out tracing info to trace file
 *
 * Input:
 *      fio		- file descriptor
 *      evnt_info        - structure containing event information (timings, etc)
 *
 * Output:
 *
 */
void
_evnt_trace_flush(struct fdinfo *fio, struct evnt_f *evnt_info)
{
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	int     status;
	int     pos_save;
	int     trc_file_number;
	struct ffsw log_stat;
	int     log_ubc = 0;

	if (evnt_info->file_ptr == NULL)
		return;

	trc_file_number = evnt_info->trc_file_number;

	pos_save = EVNT_XR_TELL();

	if (evnt_info->need_parent_child) {
		char    PROGRAM[] = "program";
		char   *parent;
		char   *child;

		if (evnt_info->parent_name == NULL)
			_evnt_get_parent_child(fio,
					      &evnt_info->parent_name,
					      &evnt_info->child_name);
		parent = evnt_info->parent_name;
		child = evnt_info->child_name;

		sprintf(evnt_info->open_info.parent_child, "%s <-> %s", 
			parent, child);

		status = EVNT_XR_SEEK(evnt_info->open_pos, SEEK_SET);
		status = EVNT_XR_WRITE(&evnt_info->open_info, 1,
				    sizeof(struct evnt_open_info));
		evnt_info->need_parent_child = FALSE;
	}

	EVNT_FLUSH_TRACE_INFO(trc_file_number);

	status = EVNT_XR_SEEK(pos_save, SEEK_SET);
#endif
}
