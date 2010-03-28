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


static char USMID[] = "@(#) libu/ffio/c1/er90breada.c	92.0	10/08/98 14:57:41";


#include <sys/types.h>
#include <sys/iosw.h>
#include <errno.h>
#include <ffio.h>
#include <liberrno.h>
#include "er90by.h"
#include "../sysio.h"

/*
 * ER90 byte stream asynchronous read
 * _er90b_reada() calls the system call reada(2), after doing appropriate
 * conversions on the parameters
 */

int
_er90b_reada(struct fdinfo *fio, bitptr bufptr, int nbytes, 
	struct ffsw *retstat, int fulp, int *ubc)
{
int ret = 0;
char *buf;
ER90BYT *f;

	buf = BPTR2CP(bufptr);
	if ((BPBITOFF(bufptr) & 7) != 0 || *ubc != 0)
		ERETURN(retstat, FDC_ERR_UBC, 0);

	if (fio->rwflag == POSITIN) {
		f = (ER90BYT *)fio->lyr_info;
		if (f->tpos){
			ret = _tape_tpwait(f->fd, &(f->tpos));
			if (ret < 0)
				ERETURN(retstat, errno, 0);
		}
	}
	else if (fio->rwflag == WRITIN) {
		ERETURN(retstat, FDC_ERR_RAWR, 0);
	}

	if (nbytes > 0) {
		CLRFFSTAT(*retstat);	/* flag async in progress */
		ret = reada(fio->realfd, buf, nbytes,
			(struct iosw *)retstat, 0);
		if (ret < 0)
			ERETURN(retstat, errno, 0);
	}
	else if (nbytes < 0) {
		ERETURN(retstat, FDC_ERR_REQ, 0);
	}
	else {	/* nbytes == 0 */
		retstat->sw_flag = 1;
		FFSTAT(*retstat) = FFCNT; /* I/O is done, and other stat */
					/* fields are set. */
	}

	fio->rwflag = READIN;
	return (ret);
}

