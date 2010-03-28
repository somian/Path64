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


#pragma ident "@(#) libu/ffio/evntclose.c	92.2	10/07/99 22:15:19"


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ffio.h>
#include "evntio.h"
#include <stdlib.h>

/*
 * _evnt_close
 *
 * Time close request
 *
 * Input:
 *      fio             - ffio file descriptor
 *      stat            - pointer to status return word
 *
 * Output:
 *      ret             - return value from backrtn
 *
 */
int
_evnt_close(struct fdinfo *fio, struct ffsw *stat)
{
	struct fdinfo *llfio;
	struct evnt_f *evnt_info;
	int     status;
	int     start_rtc, finish_rtc;
	struct ffsw log_stat;
	int     log_ubc = 0;
	int     ret;

	evnt_info = (struct evnt_f *) fio->lyr_info;
	llfio = fio->fioptr;

	if (evnt_info->parent_name == NULL)
		_evnt_get_parent_child(fio,
				      &evnt_info->parent_name,
				      &evnt_info->child_name);

	start_rtc = RTC();
	ret = XRCALL(llfio, closertn) llfio, stat);
	finish_rtc = RTC();

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	if (evnt_info->optflags.trace) {
		int     record[4];

		record[0] = (_evnt_CLOSE | evnt_info->fd);
		record[1] = start_rtc;
		record[2] = finish_rtc;
		record[3] = ret;
		status = EVNT_XR_WRITE(record, sizeof(int), 4);
		INC_GLOBAL_LOG_COUNT(close);
	}
	if (evnt_info->file_ptr)
		EVNT_XR_FLUSH();
#endif

	evnt_info->counts.close++;
	evnt_info->counts.total++;
	evnt_info->close_time = finish_rtc - start_rtc;

	if (evnt_info->optflags.diag || evnt_info->optflags.trace)
		_evnt_close_diags(fio, evnt_info, TRUE);

	_evnt_clfree(fio);
	return (ret);
}



/*
 * _evnt_clfree
 *
 * Free the memory blocks used by the event layer and set corresponding
 * pointers to NULL
 *
 * Input:
 *      fio             - ffio file descriptor
 *
 * Output:
 *
 */
void
_evnt_clfree(struct fdinfo *fio)
{
	struct evnt_f *evnt_info;
	struct evnt_async_tracker *this_tracker;
	struct evnt_async_tracker *next_tracker;

	evnt_info = (struct evnt_f *) fio->lyr_info;

	if (fio->lyr_info != NULL) {
		this_tracker = evnt_info->async_tracker;
		while (this_tracker) {
			next_tracker = this_tracker->next_tracker;
			free(this_tracker);
			this_tracker = next_tracker;
		}

		free(fio->lyr_info);
		fio->lyr_info = NULL;
	}
	if (fio->fioptr != NULL) {
		free(fio->fioptr);
		fio->fioptr = NULL;
	}

}
