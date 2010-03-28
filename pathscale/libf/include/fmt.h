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


/* USMID @(#) libf/include/fmt.h	92.1	11/10/99 13:40:53 */

#include <cray/dopevec.h>
#include <cray/format.h>

/*
 *	Mode bits for calling NICV routine (used by rnly.c and wnly.c)
 */
 
#define MDPART  040000	/* D part of D/E/F/Gw.d is present */
#define MCFT    020000	/* not called in compiler scan mode */
#define MPLS    010000	/* force output of the '+' in front of positive #'s */
#define MEXPS    04000	/* E part of D/E/F/Gw.dEe is present */
#define MSEXP    02000	/* signed exponent requested */
#define MMAN     01000	/* a double precision number is passed */
#define MD        0400	/* D format */
#define MG        0200	/* G format */
#define ME        0100	/* E format */
#define MF         040	/* F format */
#define MI         020	/* I format */
#define MX         010	/* X format */
#define MO          04	/* O format */
#define MBZ         02	/* blanks treated as zeroes */
#define MBN         01	/* blanks treated as nulls (ignored) */

/*
 *	Constants
 */

/*
 * INVALID_INTLEN is the mask value which indicates that an internal 
 * length is not supported for some edit descriptor.
 */

#define INVALID_INTLEN	((signed char) -1)

/*
 * MAX_SUP_INTLEN is the largest supported internal length (measured in 
 * bytes).
 */

#define MAX_SUP_INTLEN  16	/* best if this is a power of 2 */

/*
 * For historical reasons, use the '$' names on CRI systems.
 */
#ifdef	_CRAY

#define _RCHK	$RCHK
#define _RNOCHK	$RNOCHK
#define _WCHK	$WCHK
#define _WNOCHK	$WNOCHK

#endif

/*
 *	Macros
 */

/*
 * These macros check an edit-descriptor against a data type.
 */

#define INVALID_WTYPE(op, type) (*(long *)(&_WCHK[type-1]) & (1L << (op-1)))
#define INVALID_RTYPE(op, type) (*(long *)(&_RCHK[type-1]) & (1L << (op-1)))

/*
 *	Structures
 */

typedef struct fmtchk {

#ifndef _WORD32
	unsigned long	  	:32;		/* skip 32-bit half word */
#endif
	unsigned long	  	:18;		/* right justify */

	unsigned long	q 	:1;
	unsigned long	z 	:1;
	unsigned long	r 	:1;
	unsigned long	o 	:1;
	unsigned long	l 	:1;
	unsigned long	i 	:1;
	unsigned long	g 	:1;
	unsigned long	f 	:1;
	unsigned long	es	:1;
	unsigned long	en	:1;
	unsigned long	e 	:1;
	unsigned long	d 	:1;
	unsigned long	b 	:1;
	unsigned long	a 	:1;
} fmtchk_t;

/*
 *	Externals
 */

extern signed char	_wr_ilchk[LAST_DATA_ED][MAX_SUP_INTLEN];
extern signed char	_rd_ilchk[LAST_DATA_ED][MAX_SUP_INTLEN];
extern signed char	_rw_mxdgt[LAST_DATA_ED][MAX_SUP_INTLEN];
extern fmtchk_t		_RCHK[DVTYPE_ASCII];
extern fmtchk_t		_RNOCHK[DVTYPE_ASCII];
extern fmtchk_t		_RCHK77[DVTYPE_ASCII];
extern fmtchk_t		_RCHK90[DVTYPE_ASCII];
extern fmtchk_t		_WCHK[DVTYPE_ASCII];
extern fmtchk_t		_WNOCHK[DVTYPE_ASCII];
extern fmtchk_t		_WCHK77[DVTYPE_ASCII];
extern fmtchk_t		_WCHK90[DVTYPE_ASCII];
