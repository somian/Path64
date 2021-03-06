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


#pragma ident "@(#) libu/ffio/ffopensg.c	92.2	10/11/99 15:30:43"

#include <ffio.h>
#include <liberrno.h>
#include <stdarg.h>
#include <unistd.h>
#include <cray/mtlock.h>
#include "spec_parse.h"
#if defined(_UNICOS) || defined(__mips) || defined(_LITTLE_ENDIAN)
#include "fflock.h"
#endif

#define MAX_SPEC	128	/* arbitrary limit on size of the layer list */

/*
 *	_ffopensg
 *
 *	Open an FFIO file.  Apply -F attribute specified by "cspec", which is 
 *	of the same form as that used in assign and asgcmd, without the '-F'.
 *	("cos:20,bmx")
 *      This routine was requested by applications because they want to
 *      use the oinf field in their user layers.
 *
 *	Calling sequence:
 *
 *		fd = _ffopensg(name, flags, mode, cbits, cblks, stat, cspec, oinf);
 *
 *	Return value:
 *
 *		Positive integer (really a pointer on Cray MPP and PVP systems)
 *		on success.   -1 on failure with stat->sw_error
 *		set to the error code.
 */

int 
_ffopensg(
const char	*name,
int		flags,
mode_t		mode,
long		cbits,
int		cblks,
struct ffsw	*stat,
const char	*cspec,
struct gl_o_inf *oinf)
{
	int		retfd, length;
	_ffopen_t	fd;
	union spec_u	specs[MAX_SPEC];
	struct fdinfo	*nfio;

	if( cspec == NULL ) cspec ="" ;

	length = _parse_forstr(specs, cspec, MAX_SPEC, NO, _LELVL_RETURN);
	if (length <= 0)
		ERETURN(stat, FDC_ERR_BADSPC, 0);

	fd = _ffopen(name, flags, mode, specs, stat, cbits, cblks, NULL,
		oinf);

#if defined(_CRAY1) || defined(__mips)
	if (fd != _FFOPEN_ERR  && MULTI_ON) {
		nfio = NULL;
		if (_ff_top_lock(fd, &nfio, stat) < 0)
			fd = _FFOPEN_ERR;	
		if (nfio != NULL)
			fd = (_ffopen_t)nfio;
	}
#endif
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
        /*
         * _ffopensg returns an int. Call a routine which associates an
         * int with what is returned by _ffopen
         */
        retfd = _ff_fdinfo_to_int(fd, stat);
#else
        retfd = (int)fd;
#endif

	return(retfd);
}

