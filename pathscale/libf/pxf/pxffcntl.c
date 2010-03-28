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


#pragma ident "@(#) libf/pxf/pxffcntl.c	92.1	06/29/99 11:36:06"
#include <fortran.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <liberrno.h>
#include <stdlib.h>   /* pickup NULL definition */
#include "pxfstruct.h"
#include "table.h"

/*
 * Description:
 *	PXFFCNTL provides the functionality of fcntl(2). Currently,
 *		the only commands allowed are only F_GETLK, F_SETLK
 *		and F_SETLKW.
 * Standard:
 *	Section 6.5.2 of Posix 1003.9-1992
 * Parameters:
 *	ifildes	(input)	file descriptor
 *	icmd	(input) command
 *	iargin	(input)	command specific
 *	iargout (output) return value
 *	ierror  (output) error code
 */

#ifdef _UNICOS
void
PXFFCNTL(
#else
void
_PXFFCNTL(
#endif
	  _f_int *ifildes,
	  _f_int *icmd,
	  _f_int *iargin,
	  _f_int *iargout,
	  _f_int *ierror)
{
	int ret;
	struct pxfhandle pxfhand;

	*ierror = 0;

	switch(*icmd) {
		case F_GETLK:	
		case F_SETLK:	
		case F_SETLKW:	
		        pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table,
							  *iargin);
			if (pxfhand.pxfstructptr == NULL ||
			    pxfhand.pxftype != PXF_FLOCK) {
			  *ierror = EBADHANDLE;
			  return;
			}

			ret = fcntl(*ifildes, *icmd,
				    (struct flock *)pxfhand.pxfstructptr);
			if (ret < 0) {
				*ierror = errno;
			}
			*iargout = (_f_int)ret;
			break;
		case F_DUPFD:
		case F_SETFD:
		case F_GETFD:
		case F_GETFL:
		case F_SETFL:
#ifdef _UNICOS
		case F_SETSB:
		case F_SETALF:
		case F_CLRALF:
#endif
			ret = fcntl(*ifildes, *icmd, *iargin);
			if (ret < 0) {
				*ierror = errno;
			}
			*iargout = (_f_int)ret;
			break;
		default:
			*ierror = EINVAL;
	}
}

#ifndef _UNICOS
void
pxffcntl_(
	  _f_int *ifildes,
	  _f_int *icmd,
	  _f_int *iargin,
	  _f_int *iargout,
	  _f_int *ierror)
{
  _PXFFCNTL(ifildes,icmd,iargin,iargout,ierror);
}
#endif
