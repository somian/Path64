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


static char USMID[] = "@(#) libu/ffio/c1/bmxskipf.c	92.0	10/08/98 14:57:41";


#include  <ffio.h>
#include  <sys/types.h>
#include  <sys/param.h>
#include  <sys/bmxctl.h>
#include  <sys/stat.h>
#include  <sys/iosw.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <tapereq.h>
#include  <errno.h>
#include  "bmxio.h"
#include  <liberrno.h>
 
#define RETURN(x) { ret = x; goto done;}

/*
 *	Skip tape mark.
 */

_bmx_skipf(BMXFIL *f, long nb, int *count)
{
	register int	lib = 0;
	int lib_tpmk = 0;
	int ret = 0;
	int rescount;
	int usertm;

	*count = 0;
	ret = 0;

	if (nb == 0 ) {
		/* Position to 0 just to clear out all read-ahead */
		if (_bmx_stpos(f,FP_TPOS_BACK,0,0,0,0) < 0) {
			RETURN(-1);
		}
	}

	else if (nb > 0 ) { /* Positioning forward nb files */
		/* Position to 0 just to clear out all read-ahead */
		if ( _bmx_stpos(f,FP_TPOS_BACK,0,0,0,0) < 0) {
			RETURN(-1);
		}
		if (f->bmx_rwtpmk == 0) {
			/* No user tape marks */
			errno = FETASKPF;
			RETURN(-1);	
		}
		if ( _bmx_tpmk(f->bmx_fd, nb,  &rescount) < 0) {
			if (errno != ETEOF) {
				RETURN(-1);
			}
		}
		*count = nb - rescount;	
	}
	else {	/* Positioning back nb files */

		/* position back 1 record. If we were at BOF, then*/
		/* this will put us in the previous file */
		if (_bmx_stpos(f,FP_TPOS_BACK,1,0,0,0,&usertm) < 0) {
			if (errno == ETBOF) {
				RETURN(0);
			}
			else {
				RETURN(-1);
			}
		}
		if (f->bmx_rwtpmk == 0) {
			/* No user tape marks, just rewind */
			if (_bmx_rewd(f) < 0) {
				RETURN(-1);
			}
			RETURN(0);
		}
		
		if (_bmx_tpmk(f->bmx_fd, nb, &rescount) == 0) {
			/* Move to the other side of this tape mark */
			if ( _bmx_stpos(f,FP_TPOS_FORW,1,0,0,0) < 0) {
				RETURN(-1);
			}
		}
		else {
			if (errno != ETBOF) {
				RETURN(-1);
			}	
		}
		*count = -nb - rescount;	
		if (usertm == 0) {
			/* The user was positioned in the middle */
			/* of a file.  */
			if (*count > 0)
				*count=*count-1;
		}
	}

done:
	_bmx_clear(f);		/* Clear all lists */
	f->bmx_bufptr = 0;      /*  indicate empty buffer   */
	f->bmx_recl   = 0;      /*  and clear record length */
	return(ret);

}


/*
 * Change the tape position by nb tape marks
 */
_bmx_tpmk(int fd, int nb, int *rescount)
{

	struct dmn_comm	ctl;

	ctl.POS_REQ = TR_PTMS;
	ctl.POS_COUNT = nb;

	*rescount = 0;	/* Residual count */
	if ( ioctl( fd, BXC_SPOS, &ctl) < 0 ){
		return(-1);
	}

	ctl.POS_REP = 0;
	if ( ioctl( fd, BXC_GPOS, &ctl) < 0 ){
		return(-1);
	}

	*rescount = ctl.POS_COUNT;
	if (ctl.POS_REP != 0){
		errno = ctl.POS_REP;
		return(-1);
	}
	return(0);
}
