/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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



#pragma ident "@(#) libf/fio/dsetpos.c	92.1	06/18/99 10:21:14"

#include <foreign.h>
#include <errno.h>
#include <liberrno.h>
#include "fio.h"

/*
 *	internal setpos routine
 */
int
_setpos(
FIOSPTR	css,		/* statement state */
unit	*cup,		/* unit pointer */
int	*pa,		/* original user's Position Array. maybe tape... */
int	len)		/* length of pa */
{
	int	ret;
	int	pos;
	long	offset;
	int	whence;
	int	retstat;
	struct fdinfo *fio;
	int	fp_parm[2];

	retstat = 0;

/*
 *	Wait for completion of a preceding asynchronous BUFFER IN/OUT.
 */
	WAITIO(cup, return(cup->uffsw.sw_error));

	cup->urecpos = 0;
	if (cup->uwrt) {
		if (cup->utrunc) {
			ret = _unit_trunc(cup);
			if (ret != 0)
				return(ret);
		}
		cup->uwrt = 0;
	}
/*
 *      According to the file structure make the appropriate call to set
 *      the file position.  Positioning routines are file structure dependent.
 */
	pos = pa[0];
	switch( cup->ufs ) {

	case  FS_TEXT:
	case  STD:
		if ( pos == -1 || pos == -2 ) {
			offset = 0;
			whence = 2;
		}
		else {
			offset = pos; 
			whence = 0;
		}
/*
 *		If unformatted file, then positioning is done to a word
 *		boundary.  Converto to bytes
 */
		if ( cup->ufmt == NO )
			offset = _dshiftl(offset, offset, 3);

		ret = fseek(cup->ufp.std, offset, whence);
		if (ret != 0)
			return(errno);

		break;

	case  FS_FDC:
		fio = cup->ufp.fdc;
		
		if ((cup->uflagword & FFC_SEEKA) ||
		    ((pos == -1 || pos == -2) && (cup->uflagword & FFC_SEEKE))){

			if ( pos == -1 || pos == -2 ) {

				if ( XRCALL(fio, seekrtn) fio, 0, 2,
							   &cup->uffsw) < 0)
					return(cup->uffsw.sw_error);

			}
			else {
#if	defined(__mips) || defined(_LITTLE_ENDIAN)

        /* KEY: this comment says that we should be able to implement */
	/* this now that file offsets are 64 bits, but this routine is confusing. */

	/* A couple of problems will need to be solved before we can */
	/* use the ffio posrtn on MIPS. That routine is currently */
	/* documented as expecting longs in the 3rd parameter for */
	/* FP_BSEEK . But more importantly, FP_BSEEK isn't supported */
	/* yet for MIPS, because  longs are not sufficiently large */
	/* to contain all possible file positions. */
				return(FDC_ERR_NOSUP);
#else
/*
 *				If formatted file, then positioning is done
 *				to a byte boundary.  If unformatted, input pos
 *				is in words.  Convert to bits.
 */
				if ( cup->ufmt == YES )
					fp_parm[0] = _dshiftl(pos, pos, 3);
				else
					fp_parm[0] = _dshiftl(pos, pos, 6);
/*
 *				Check for Invalid argument.  If shifted result
 *				is negative, we have a problem.
 */
				if ( fp_parm[0] < 0 )
					return(FEBIOISP);
					
				fp_parm[1] = 0;
				ret = XRCALL(fio, posrtn) fio, 
					FP_BSEEK, fp_parm, 2, &cup->uffsw);

				if (ret < 0)
					return(cup->uffsw.sw_error);
#endif
			}
		}
		else {
/*
 *			Assume that positioning is done via a 'magic cookie'.
 *			Just pass it through.
 */
			if( XRCALL(fio, posrtn)
				fio, FP_SETPOS, pa, len, &cup->uffsw) < 0)
				return(cup->uffsw.sw_error);
		}

		break;

	case FS_AUX:
		return(FEMIXAUX);
	default:
		return(FEINTFST);
	}
/*
 *	Set the end flag if going to EOD, else clear it.
 */
	if (pos == -1 && len == 1) {
		if (cup->ufs != FS_FDC)
			cup->uend = LOGICAL_ENDFILE;
		else {
			/*
			 * We are after the terminal endfile record of the file,
			 * but we must decide if it's a logical or physical 
			 * endfile record.
			 */
			if ((cup->uflagword & FFC_WEOF) == 0)
				cup->uend = LOGICAL_ENDFILE;
			else {
				off_t fbytepos;	/* byte offset within file */

				switch (fio->class) {
				case CLASS_COS:
					/*
					 * Since seekrtn for the cos layer
					 * when called with 0,2 does NOT return
					 * the resulting byte offset, we
					 * must query the position with this
					 * seekrtn call.  The only case
					 * where 0 might be returned is when 						 * the file is empty.
					 */
					fbytepos = XRCALL(fio, seekrtn) fio, 0,
						1, &cup->uffsw);
					if (fbytepos > 0)
						cup->uend = PHYSICAL_ENDFILE;
					else
						cup->uend = LOGICAL_ENDFILE;
					break;
				default:
					return(FEBIOSNT);
				}
			}
		}
	}
	else
		/* 
		 * Sadly, we may be positioned after an endfile record, 
		 * and not know it.   This should be fixed. 
		 */
		cup->uend = BEFORE_ENDFILE;
	return(OK);
}
