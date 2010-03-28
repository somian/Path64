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


#pragma ident "@(#) libu/ffio/tmfskipf.c	92.1	06/29/99 13:16:47"


#include "tmfio.h"

/*
 *	Tape positioning requests for Irix
 */

/*
 * Change the tape position by ntpmk tape marks 
 * Note that this routine does not set stat.sw_stat unless
 * there is an error. We could probably set it to EOD and BOD,
 * but setting it to EOF is ???
 * Skips the specified number of tape marks. If skipping forward,
 * it will position you directly after a user tape mark or at EOD.
 * If skipping backwards, it will position you directly before a
 * user tape mark or at BOD
 */
int
_tmf_skiptpmk(
	int		fd,
	int		ntpmk,
	int		*rescnt,
	struct ffsw	*stat)
{
	tmfpfm_t	ctl;

	*rescnt		= 0;	/* residual count */
	bzero(&ctl, sizeof(ctl));
	ctl.rh.request	= TR_PFMS;
	ctl.rh.length	= sizeof(tmfpfm_t) - sizeof(tmfreqhdr_t);
	ctl.count	= ntpmk;

	if (ioctl(fd, TMFC_DMNREQ, &ctl) < 0) {
		ERETURN(stat, errno, 0);
	}

	*rescnt	= ctl.rh.residual;

	if (ctl.rh.reply != 0) {
		ERETURN(stat, ctl.rh.reply, 0);
	}
		
	return(0);
}

/*
 * Skip file.
 * Note that this has slightly different meaning that skip tape mark.
 *	Except in the case where you request positioning past EOD,
 *	_tmp_skipf will position you at the beginning of a file. That is,
 *	_tmf_skipf will position you either at BOD, immediately after a
 *	user tape mark, or at EOD.
 */
int
_tmf_skipf(
	struct fdinfo	*fio,
	long		nb,
	int		*count,
	struct ffsw	*stat)
{
	int		rescnt;
	int		usertm;
	struct tmfio	*xf_info;

	xf_info	= (struct tmfio *)fio->lyr_info;
	*count	= 0;

	if (nb == 0) {
		/* Position to 0. This will cause us to wait for */
		/* any outstanding positioning, to flush any unwritten data, */
		/* and if we're ever doing */
		/* read-ahead it will clear that out. */

		if (_tmf_stpos(fio, FP_TPOS_BACK, 0, 0, 0, 0, &usertm, stat) < 0) {
				return(ERR);
		}
		return(0);
	}
	else if (nb > 0) {
		/* Positioning forward nb files.*/
		/* First position to 0. This will cause us to wait for */
		/* any outstanding positioning, to flush any unwritten data, */
		/* and if we're ever doing read-ahead it will */
		/* clear that out. */
		if (_tmf_stpos(fio, FP_TPOS_BACK, 0, 0, 0, 0, &usertm, stat) < 0) {
				return(ERR);
		}
		if (xf_info->tmf_rwtpmk == 0) {
			/* No user tape marks */
			ERETURN(stat, FETASKPF, 0);
		}
		if (_tmf_skiptpmk(xf_info->tmf_fd, nb, &rescnt, stat) < 0) {
			if (stat->sw_error != ETEOF) {
				return(ERR);
			}
		}
		*count	= nb - rescnt;
	}
	else {
		/* Positioning back nb files */
		/* First position back 1 record. If we were */
		/* immediately after a tapemark, then this */
		/* will put us in the previous file. */
		if (_tmf_stpos(fio, FP_TPOS_BACK, 1, 0, 0, 0, &usertm, stat) < 0) {
			if (stat->sw_error == ETBOF) {
				return(0);
			}
			return(ERR);
		}
		if (xf_info->tmf_rwtpmk == 0) {
			/* No user tape marks, just rewind */
			if (_tmf_seek(fio,0,0,stat) < 0) {
				*count	= -nb;
				return(ERR);
			}
			*count	= 0;
			return(0);
		}
		/* Now position back nb files */
		if (_tmf_skiptpmk(xf_info->tmf_fd, nb, &rescnt, stat) == 0) {
			/* Move to the other side of this tape mark */
			int	ut;
			if (_tmf_stpos(fio, FP_TPOS_FORW, 1, 0, 0, 0, &ut, stat) < 0) {
				return(ERR);
			}
		}
		else {
			if (stat->sw_error != ETBOF) {
				return(ERR);
			}
		}

		*count	= -nb - rescnt;
		if (usertm == 0) {
			/* The user was positioned in the middle of a file. */
			if (*count > 0)
				*count	= *count - 1;
		}

	}
	return(0);
}
