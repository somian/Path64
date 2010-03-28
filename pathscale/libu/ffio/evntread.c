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


#pragma ident "@(#) libu/ffio/evntread.c	92.2	10/07/99 22:15:19"


#include <stdio.h>
#include <errno.h>
#include <ffio.h>
#include "evntio.h"

/*
 * _evnt_read
 *
 * Log a read request
 *
 * Input:
 *      fio     	- ffio file descriptor
 *	bufptr		- bit pointer to where data is to go
 *	nbytes		- number of bytes to be read
 *	stat		- pointer to status return word
 *	fulp		- full or partial read mode flag
 *	ubcp		- pointer to unused bit count.  On return,
 *			  *ubcp is updated to contain the unused bit
 *			  count in the data returned.
 *
 * Output:
 *	ret		- return value from readrtn
 */
ssize_t
_evnt_read(struct fdinfo *fio, bitptr bufptr, size_t nbytes, 
	   struct ffsw *stat, int fulp, int *ubcp)
{
	struct fdinfo 	*llfio;
	struct evnt_f 	*evnt_info;
	int     	status;
	rtc_t     	start_rtc, finish_rtc;
	ssize_t     	ret;
	struct ffsw 	log_stat;
	int     	log_ubc = 0;

	evnt_info = (struct evnt_f *) fio->lyr_info;
	llfio = fio->fioptr;

	EVNT_UPDATE(read);

	start_rtc = RTC();
	ret = XRCALL(llfio, readrtn) llfio, bufptr, nbytes, stat, fulp, ubcp);
	finish_rtc = RTC();

	evnt_info->cur_pos += nbytes;
	if (evnt_info->cur_pos > evnt_info->cur_size)
		evnt_info->cur_pos = evnt_info->cur_size;
	EVNT_CHECK_SIZE;

#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	if (evnt_info->optflags.trace) {
		int     record[4];

		EVNT_LOCK_ON;
		record[0] = (_evnt_READ | evnt_info->fd) | nbytes;
		record[1] = start_rtc;
		record[2] = finish_rtc;
		record[3] = ret;
		status = EVNT_XR_WRITE(record, sizeof(int), 4);
		INC_GLOBAL_LOG_COUNT(read);
		EVNT_LOCK_OFF;
	}
#endif
	evnt_info->counts.total++;
	evnt_info->counts.read++;
	evnt_info->read.delivered += ret;
	evnt_info->read.time += (finish_rtc - start_rtc);

	return (ret);
}
