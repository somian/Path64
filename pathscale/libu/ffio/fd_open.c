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


#pragma ident "@(#) libu/ffio/fd_open.c	92.2	10/07/99 22:16:08"

#include <stdio.h>
#if defined(_ABSOFT)
#include "ac_sysdep.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ffio.h>
#include "sysio.h"
#include "fdio.h"
#include "fxlist.h"

#if defined(_CRAY) || defined(__mips) || defined(_LITTLE_ENDIAN)
DECLARE(FD_XLIST);
struct xtr_s FD_XLIST_P = { FD_XLIST };

/*
 * FD open routine.  Open a user specified file descriptor, and set up the
 *	info block accordingly.  No info block is set up per-se.  If we
 *	are in this routine, then the user has ended the spec with
 *	this layer.  Build a spec with the system layer followed by
 *	this one.  This will allow the gsys_open routine to do its
 *	work and handle the fd according to the device type.
 */

_ffopen_t
_fd_open(
const char	*name,
int		flags,
mode_t		mode,
struct fdinfo	*fio,
union spec_u	*spec,
struct ffsw	*retstat,
long		cbits,
int		cblks,
struct gl_o_inf *oinf)		/* global open information for this file */
{
	union spec_u newspec[3];

	newspec[0].wword	= 0;
	newspec[0].fld.ext	= 0;
	newspec[0].fld.class	= CLASS_SYSTEM;

	newspec[1]		= spec[0];

	newspec[2].wword	= 0;

	return(_gsys_open(name, flags, mode, fio, newspec, retstat, cbits,
			cblks, oinf));
}
#endif


/*
 *	_fd_check will check the flags on a file descriptor and determine
 *	if the parameters passed will actually work.  The main job of this
 *	routine is to determine if the flags on the fd match the R/W
 *	flags passed in.
 */
int
_fd_check(
const char	*name,
int		flags,
mode_t		mode,
struct fdinfo	*fio,
union spec_u	*spec,
struct ffsw	*retstat,
long		cbits,
int		cblks,
struct gl_o_inf *oinf)		/* global open information for this file */
{
	int fd;
	int oldflags;

	fd = FD(spec);
/*
 *	Check to make sure that the read/write permissions on the
 *	file are the same as that on the requested file descriptor.
 */
	oldflags = fcntl(fd, F_GETFL);
	if ((oldflags & O_ACCMODE) == (flags & O_ACCMODE))
		return(fd);
	else
		ERETURN(retstat, EACCES, 0);
}
