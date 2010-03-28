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


#pragma ident "@(#) libu/ffio/tmftptsi.c	92.1	06/29/99 13:16:47"

#include  "tmfio.h"
#include  "sysio.h"

/* 
 * Issue request to get tape info 
 */
int
_tmf_tptsi(struct tmfio *xf_info, struct tsdata *tsi, char vsn[][L_MAXVSN])
{
	register int	remsize;
	register int	ret;
	char		*tmpbuf;
	char		*repptr;
	tmfinfo_t	tpdinfo;

	/* We can get this info with an ioctl */

	(void) memset(&tpdinfo, 0, sizeof(tpdinfo));

	if ((tmpbuf = malloc(sizeof(struct tsdata) + MAXVSN*L_MAXVSN)) == NULL){
		errno	= FDC_ERR_NOMEM;
		goto errret;
	}

	tpdinfo.databuf		= tmpbuf;
	tpdinfo.datalen		= sizeof(struct tsdata) + MAXVSN*L_MAXVSN;
	tpdinfo.rh.request	= TR_INFO;
	tpdinfo.rh.length	= sizeof(tpdinfo) - sizeof(tmfreqhdr_t);

	/* Request the data */

	LOOP_SYSCALL(ret, ioctl(xf_info->tmf_fd, TMFC_DMNREQ, &tpdinfo));

	if (ret < 0) {
		return(ERR);
	}
	if (tpdinfo.rh.reply != 0) {
		errno	= tpdinfo.rh.reply;
		return(ERR);
	}

	(void) memcpy(tsi, tmpbuf, sizeof(struct tsdata));

	repptr	= tmpbuf + sizeof(struct tsdata);

	/* Copy the vsn list */

	if (vsn != NULL) {
		remsize	= tpdinfo.datalen - sizeof(struct tsdata);	
		if (remsize > MAXVSN * L_MAXVSN) {
			remsize	= MAXVSN * L_MAXVSN;
		}
	        (void) memcpy(vsn, repptr, remsize);
	}
        free(tmpbuf);
       	return(0);
		
errret:
	return(ERR);
}
