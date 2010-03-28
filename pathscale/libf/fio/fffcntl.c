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



#pragma ident "@(#) libf/fio/fffcntl.c	92.1	06/18/99 16:08:47"

/*
 *	Provide access to fffcntl call for FORTRAN units
 *
 *	>=0 OK (depends on fffcntl return)
 *	<0  bad call. (-2 means too few params)
 *		if *ustat = -1, unit was not open as FDC, or not open.
 */

#include <foreign.h>
#include <errno.h>
#include "fio.h"
 
long
FFFCNTL(unitnum, cmd, arg, len, ustat)
long 	*unitnum;
long 	*len;
long 	*cmd;
long 	*arg;
long 	*ustat;
	{
	unit 	*cup;
	int	ret;
	struct ffsw stat;

	if (_numargs() < 5)
		return(-2);	/* too few parameters */

	*ustat = -1;	/* assume unit not open...*/
	STMT_BEGIN(*unitnum, 0, T_MISC, NULL, NULL, cup);	/* lock unit */
	if (cup == NULL)
		return(-1);	/* invalid or unconnected unit */

	if (cup->ufs != FS_FDC) {
		ret = -1;
		goto done;
	}

	*ustat = 0;
/*
 *	If command code is zero, just return fdinfo pointer.
 */
	if (*cmd == 0) {
		*arg = (long)cup->ufp.fdc;
		ret  = 0;
		goto done;
	}

	ret = XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc, *cmd, arg, &stat);
	if (ret < 0)
		*ustat = stat.sw_error;

done:
	STMT_END(cup, T_MISC, NULL, NULL);
	return(ret);
}
