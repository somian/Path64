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


#pragma ident "@(#) libu/ffio/evntflush.c	92.2	10/07/99 22:15:19"


#include <stdio.h>
#include <errno.h>
#include <ffio.h>
#include "evntio.h"

/*
 * _evnt_fcntl
 *
 * event layer flush requests
 *
 * Input:
 *      fd      - file descriptor (dummy)
 *      iostat  - pointer to status return word
 *
 * Output:
 *	ret	- return value from flushrtn
 */
int
_evnt_flush(struct fdinfo *fio, struct ffsw *iostat)
{
	struct fdinfo *llfio;
	struct evnt_f *evnt_info;
	int     status;
	rtc_t     start_rtc, finish_rtc;
	struct ffsw log_stat;
	int     log_ubc = 0;
	int     ret;

	evnt_info = (struct evnt_f *) fio->lyr_info;
	llfio = fio->fioptr;

	start_rtc = RTC();
	ret = XRCALL(llfio, flushrtn) llfio, iostat);
	finish_rtc = RTC();

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	if (evnt_info->optflags.trace) {
		int     record[4];

		EVNT_LOCK_ON;
		record[0] = _evnt_FLUSH | evnt_info->fd;
		record[1] = start_rtc;
		record[2] = finish_rtc;
		record[3] = ret;
		status = EVNT_XR_WRITE(record, sizeof(int), 4);
		INC_GLOBAL_LOG_COUNT(flush);
		EVNT_LOCK_OFF;
	}
#endif
	evnt_info->counts.flush++;
	evnt_info->counts.total++;
	evnt_info->flush_time += (finish_rtc - start_rtc);

	return (ret);
}
