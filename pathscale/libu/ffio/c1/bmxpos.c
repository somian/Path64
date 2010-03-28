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


static char USMID[] = "@(#) libu/ffio/c1/bmxpos.c	92.0	10/08/98 14:57:41";


#include <ffio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <errno.h>
#include "bmxio.h"
#define BMXLEN 13

/*
 *	TAPE positioning requests
 */

_bmx_pos(struct fdinfo *fio, int cmd, long *arg, int len, struct ffsw *stat)
{
BMXFIL *f;
long bmxtab[BMXLEN];
int loc_f_count;
int rescount;
struct ffp_settp_s *pos;
struct ffp_skipf_s *skp;
struct ffp_skiptpmk_s *tpmk;
long abspos;
struct  ffp_abs *ffpabs;

	f = (BMXFIL *)fio->lyr_info;
	switch(cmd)
		{
		case FP_GETPOS:
			if (f->er90) {
				ERETURN(stat,FDC_ERR_NOSUP,0);
			}
			if (len < 2)
				ERETURN(stat,FEBIOSNT,0);
			/* Return VSN of last block processed and 
			 * the block number of the last block processed.
			 * If we were reading, block number is calculated
			 * from the physical block position - (blocks in
			 * buffer memory + blocks in library ). If we
			 * were writing, block number is physical block since
			 * we requested synch.
			 */
			if( _bmx_gtpos(f, bmxtab, BMXLEN, 1)) {
				ERETURN(stat, errno, 0);
			}
			*arg++ = bmxtab[0];
			if (f->bmx_flag & BMXIO_RW)
				*arg = bmxtab[9] + 1;	/* writing */
			else
				*arg = bmxtab[9] - (bmxtab[10]+bmxtab[11]) + 1;
			break;
		case FP_SETPOS:
			if (f->er90) {
				ERETURN(stat,FDC_ERR_NOSUP,0);
			}
			/* Set position */
			if (len < 2)
				ERETURN(stat,FEBIOSNT,0);
			if (*arg == -1)
				ERETURN(stat,FEBIOSNT,0);
			if (_bmx_stpos(f, FP_TPOS_ABS, *(arg+1),0,0,*arg))
				ERETURN(stat, errno, 0);
			fio->rwflag = POSITIN;
			break;
		case FP_SKIPF:
			skp = (struct ffp_skipf_s *)arg;
			if ( _bmx_skipf(f, skp->ffp_nfil, &loc_f_count))
				ERETURN(stat,errno,0);
			skp->ffp_nfil = loc_f_count;
			skp->ffp_nrec = 0;
			fio->rwflag = POSITIN;
			break;
		case FP_SETTP:
			pos = (struct ffp_settp_s *)arg;
			if (_bmx_stpos(f, pos->ffp_nbs_p, pos->ffp_nb, 
				pos->ffp_nvs_p, pos->ffp_nv, pos->ffp_vi))
				ERETURN(stat,errno,0);
			fio->rwflag = POSITIN;
			break;	
		case FP_SKIPTPMK:
			tpmk = (struct ffp_skiptpmk_s *)arg;
			/* Position to 0 to clear out all read-ahead */
			if (_bmx_stpos(f, FP_TPOS_BACK,0,0,0,0))
				ERETURN(stat,errno,0);
			if (_bmx_tpmk(f->bmx_fd, tpmk->ffp_ntpmk, &rescount))
				ERETURN(stat, errno, 0);
			tpmk->ffp_ntpmk = rescount;
			fio->rwflag = POSITIN;
			break;
		case FP_GABS:	
			/* According to the definition of this ioctl, */
			/* we do not adjust for read-ahead.*/
			if (f->er90) {
				if (_bmx_gabs(f, (struct ffp_abs *)arg) < 0)
					ERETURN(stat, errno, 0);	
			}
			else {
				if (_bmx_gabs(f, &abspos) < 0)
					ERETURN(stat, errno, 0);	
				ffpabs = (struct  ffp_abs *)arg;
				ffpabs->ffp_absaddr = abspos;
			}
			break;
		case FP_SABS:	
			if (f->er90) {
				if (_bmx_sabs(f, (struct ffp_abs *)arg) < 0)
					ERETURN(stat, errno, 0);	
			}
			else {
				if (_bmx_sabs(f,
				 ((struct ffp_abs *)arg)->ffp_absaddr) < 0)
					ERETURN(stat, errno, 0);	
			}
			fio->rwflag = POSITIN;
			break;
		default:
			ERETURN(stat, FDC_ERR_INTERR, 0);
		}
		return(0);
}
