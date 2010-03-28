/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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



#pragma ident "@(#) libf/fio/unittrunc.c	92.1	06/18/99 18:38:26"
 
#include <errno.h>
#include <liberrno.h>
#include <foreign.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include "fio.h"
#if	defined(__mips) || (defined(_LITTLE_ENDIAN) && defined(__sv2))
typedef long long _ftelltype;
#define LIBFSEEK fseek64 
#define LIBFTELL ftell64 
#define LIBFTRUNC ftruncate64
#else
#ifdef KEY /* Bug 1678 */
/* Need 64 bit position to support long files */
typedef off_t _ftelltype;
#define LIBFSEEK fseeko
#define LIBFTELL ftello
#define LIBFTRUNC ftruncate /* Vanilla Linux ftruncate already uses off_t */
#else /* KEY Bug 1678 */
typedef long _ftelltype;
#define LIBFSEEK fseek
#define LIBFTELL ftell
#define LIBFTRUNC ftruncate
#endif /* KEY Bug 1678 */
#endif
/*
 *	_unit_trunc()
 *
 *		Truncate a file at its current position. 
 *
 *	Return value
 *
 *		0 for OK
 *		error code if an error is encountered
 */
int
_unit_trunc(unit *cup) 
{
	_ftelltype flength;
	FILE	*iop;
 	
	if (cup->useq == 0)	/* If direct access file */ 
		return(0);	/* Don't truncate direct access files */

	switch(cup->ufs) {

	case FS_TEXT:
	case STD:

/*
 *		Truncate the file with trunc(2) if the file is a a regular 
 *		file.  The trunc(2) system call is not allowed on terminal 
 *		files, FIFO piped files, "/dev/null", or sockets.
 */
		if (cup->useek) {
			iop = cup->ufp.std;

			/*
			 * If stream is in O_APPEND mode (as when 'a.out>>ofil')
			 * we call fseek to current position.  Turns out that on
			 * SysV systems, ftell does not flush the buffer.  Thus
			 * the file offset does not get bumped to EOF as the
			 * result of the flush.
			 */
			if (LIBFSEEK(iop, 0, SEEK_CUR) != 0)
				return(errno);

			flength = LIBFTELL(iop);	/* get current position */

			/* position fd to the location of the truncation */
			if (LIBFSEEK(iop, flength, SEEK_SET) != 0)
				return(errno);
#ifdef	_UNICOS
			if (trunc(fileno(iop)) == -1L) 
#else
			if (LIBFTRUNC(fileno(iop), flength) == -1) 
#endif
				return(errno);
#ifdef KEY /* Bug 5386 */
			/* Until Fedora Core 3, this code never encountered
			 * a Unix system that required an "fflush" here. But
			 * without it, the FC3 stdio code doesn't notice that
			 * we've bypassed stdio and truncated the file, and
			 * to be honest, it seems like blind dumb luck that
			 * other systems haven't encountered a problem here.
			 */
			fflush(iop);
#endif /* KEY Bug 5386 */
		}
		break;

	case FS_FDC:
		if (XRCALL(cup->ufp.fdc, weodrtn)cup->ufp.fdc,
			&cup->uffsw) < 0)
			return(cup->uffsw.sw_error);
		break;

	default:
		return(FEINTFST);
	}

	return(0);
}
