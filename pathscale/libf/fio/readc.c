/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/readc.c	92.3	06/21/99 10:37:55"

/*
 *	read characters
 */

#include <errno.h>
#include <foreign.h>
#include <liberrno.h>
#include "fio.h"

#define BLANK (long) ' '

#define ERET(err) {	\
	chars	= 0;	\
	errn	= err;	\
	goto done;	\
}

static void __READC();

/*
 *	Read characters, full record mode
 */

void
READC(
	_f_int	*unump,
	_f_int	*uda,
	_f_int	*charsp,
	_f_int	*status )
{
	__READC(FULL, unump, uda, charsp, status);

	return;
}
 
/*
 *	Read characters, partial record mode
 */
 
void
READCP(
	_f_int	*unump,
	_f_int	*uda,
	_f_int	*charsp,
	_f_int	*status )
{
	__READC(PARTIAL, unump, uda, charsp, status);

	return;
}
/*
 *	Read characters, full or partial record mode;
 *
 *	The READ/READP interface does not advance past logical endfile 
 *	records.
 */
static void
__READC(
	int	fulp,
	_f_int	*unump,
	_f_int	*uda,
	_f_int	*charsp,
	_f_int	*status)
{
	register int	ret;
	register int	errn;
	long		rstat;
	long		chars;
	unum_t		unum;
	unit		*cup;
	struct fiostate	cfs;

	chars	= *charsp;
	unum	= *unump;
	errn	= 0;

	STMT_BEGIN(unum, 0, T_RSF, NULL, &cfs, cup);
/*
 *	If not connected, do an implicit open.  
 */
	if (cup == NULL) {
		int	ostat;

		cup	= _imp_open(&cfs, SEQ, FMT, unum, 1, &ostat);

		if (cup == NULL)
			ERET(ostat);
	}

 	if (!cup->ok_rd_seq_fmt) {
		errn	= _get_mismatch_error(1, T_RSF, cup, &cfs);
		ERET(errn);
	}

	cup->uwrt	= 0;

        ret	= _frch(cup, (long *) uda, chars, fulp, &rstat );

	if ( ret == IOERR ) {
		if (errno == FETAPUTE) {
			/* This should be fixed, so that chars */
			/* returns the amount of data read */
			errn	= 4;
		}
		else if (errno > 5) {
			chars	= 0;
			errn	= errno;
		}
		else{
			chars	= 0;

			/* Map system errnos 1-4 to library */
			/* errnos. Otherwise, we would lose them. */
			switch (errno) {
				case 1:
					errn	= FEKLUDG1;
					break;
				case 2:
					errn	= FEKLUDG2;
					break;
				case 3:
					errn	= FEKLUDG3;
					break;
				case 4:
					errn	= FEKLUDG4;
					break;
			}
		}
		goto done;
	}
	  
	if (ret < chars)
		(void) _memwset(&uda[ret], BLANK, chars - ret);

	if ( rstat == EOR ) {
		cup->uend	= BEFORE_ENDFILE;
		errn		= 0;		/* EOR */
		if ( ret == 0 ) 
			errn	= 1;	/* NULL record */
		chars	= ret;		/* number of characters read */
	}
	else if ( rstat == CNT ) { 
		cup->uend = BEFORE_ENDFILE;
		errn	= -1;		/* CHARACTERS REMAIN in record */
		chars	= ret;		/* number of characters read */
	}
	else if (rstat == EOD) {
		errn	= 3;		/* EOD */
		chars	= 0;		/* number of characters read */
	}
	else {
		cup->uend = PHYSICAL_ENDFILE;
		errn	= 2;		/* EOF */
		chars	= 0;		/* number of characters read */
	}

done:

	*charsp	= chars;
	*status	= errn;

	STMT_END(cup, TF_READ, NULL, &cfs);

	return;
}
