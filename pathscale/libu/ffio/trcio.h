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


/* USMID @(#) libu/ffio/trcio.h	92.0	10/08/98 14:57:41 */


#define TRC_OPEN	1
#define TRC_READ	2
#define TRC_READA	3
#define TRC_READC	4
#define TRC_WRITE	5
#define TRC_WRITEA	6
#define TRC_WRITEC	7
#define TRC_CLOSE	8
#define TRC_FLUSH	9
#define TRC_WEOF	10
#define TRC_WEOD	11
#define TRC_SEEK	12
#define TRC_BKSP	13
#define TRC_POS		14
#define TRC_ALGN	15	/* unused */
#define TRC_FCNTL	16
#define TRC_LISTIO	17

#define HIST_INIT	1024	/* words */
#define HIST_INCR	1024	/* increment */
#define HIST_MAX	1048576	/* max size */
#define HIST_BKT	512*8	/* one sector */

#define PFULP(fio, fulp)					\
			{					\
			char *str;				\
								\
			str = "PARTIAL";			\
			if (fulp == FULL)			\
				str = "FULL";			\
			_trc_info(fio, str, 0);			\
			}

#define UPDPOS(fio, ret, optype)					\
		{							\
		struct trace_f *trc_info;				\
									\
		trc_info = (struct trace_f *)fio->lyr_info;		\
/*									\
 *		Update counters						\
 */									\
		trc_info->optype++;					\
									\
		if (trc_info->lastseek == NO)				\
			trc_info->forw++;				\
		else if (trc_info->curpos == trc_info->last_endpos)	\
			trc_info->forw++;				\
		else if (ret >= 0 && 					\
			((trc_info->curpos + ret) == trc_info->last_stpos)) \
			trc_info->backw++;				\
		else	/* lastseek is YES */				\
			{						\
			if (trc_info->curpos == -1)			\
				trc_info->unknown++;			\
			}						\
/*									\
 *		Update position						\
 */									\
		trc_info->last_stpos = trc_info->curpos;		\
		if (trc_info->last_stpos >= 0 && ret >= 0)		\
			trc_info->last_endpos =	trc_info->last_stpos + ret; \
		else							\
			trc_info->last_endpos = -1; /* unknown */	\
		trc_info->curpos = trc_info->last_endpos;		\
									\
/*									\
 *		Update histogram					\
 */									\
		/* Hist stuff */					\
									\
		}

struct trace_f
	{
	char	*name;		/* name of the file (for overflow) */
	int	trcfd;		/* file descriptor of trace file */
	char	prtbuf[512];	/* print buffer */
	int	bufptr;		/* print buffer pointer */
	int	*hist;		/* Pointer to Histogram */
	int	histsiz;	/* Histogram current size */
	int	histmax;	/* Histogram max size */
	int	histincr;	/* Histogram increment size */
	int	histbkt;	/* bucket size in bytes */
	int	curpos;		/* current byte position */
	int	lastseek;	/* flag that last seek could have moved pos */
	int	last_stpos;	/* remembered start position */
	int	last_endpos;	/* remembered end position */
	int	forw;		/* forward seq counter */
	int	backw;		/* backward seq counter */
	int	unknown;	/* unknown counter (successive reada) */
	int	reads;
	int	readas;
	int	writes;
	int	writeas;
	int	lseeks;
	};
