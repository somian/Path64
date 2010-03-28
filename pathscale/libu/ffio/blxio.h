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


/* USMID @(#) libu/ffio/blxio.h	92.1	08/18/99 14:45:43 */

#define CHAR_LEN 	8	

#define BLX_COS		1	/* set up for COS default blank expansion */
#define BLX_CTSS	2	/* set up for CTSS blank expansion */

#define NUM_BLX_TYPES	2

/*
 * BLX class structure to the hold memorable information.
 */
struct blx_info
	{
	int	u_blx;	/* unused blank count for current record in bytes! */

	unsigned
		eor:1,		/* 1 if FFEOR was received from lower layer
				 * (i.e., when fio->_cnt reaches 0, we have 
				 * the "real" end-of-record), otherwise it 
				 * should be 0 */
		llblocked:1;	/* lower layer returns FFC_REC from GETINFO*/

	long	blx_char;	/* the blx character */
	int	blx_off;	/* the offset to subtract from the byte
				 * following the blx_char */
	unsigned long	cmp_char;	/* character(s) to be compressed */
	};
/*
 * INSRT_BL places blanks in the UDA.  The lesser of the requested blanks and
 * the number left in the (xblx_dat)->u_blx are "put" in the UDA.  The "ret"
 * holds the number of bits actually tranferred.
 */
#define INSRT_BL(ret, xbufptr, xblx_dat, xbits)				\
	{								\
	int ubx_temp;							\
	char *t_ptr;							\
									\
	ubx_temp = (xblx_dat)->u_blx << 3;				\
	t_ptr = BPTR2CP((xbufptr));					\
									\
	if ((xbits) >= ubx_temp)					\
		{							\
		t_ptr = memset(t_ptr,(xblx_dat)->cmp_char >> 56,	\
			(xblx_dat)->u_blx);				\
		(ret) = ubx_temp;					\
		SET_BPTR((xbufptr), CPTR2BP(t_ptr));			\
		SET_BPTR((xbufptr), INC_BPTR((xbufptr),(ret)));		\
		(xblx_dat)->u_blx = 0;					\
		}							\
	else								\
		{							\
		t_ptr = memset(t_ptr,					\
			(xblx_dat)->cmp_char >> 56,(xbits) >> 3);	\
		(ret) = (xbits);					\
		SET_BPTR((xbufptr), CPTR2BP(t_ptr));			\
		SET_BPTR((xbufptr), INC_BPTR((xbufptr),(ret)));		\
		(xblx_dat)->u_blx -= (ret) >> 3;			\
		}							\
	}


