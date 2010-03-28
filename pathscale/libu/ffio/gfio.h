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


/* USMID @(#) libu/ffio/gfio.h	92.0	10/08/98 14:57:41 */

#ifndef _GFIO_H
#define _GFIO_H
/* CLASS_F */
#define TR_IBM_F	1	/* IBM F format */
#define TR_IBM_FB	2	/* IBM FB format */
#define TR_VMS_F_DSK	3	/* VMS F.DISK format */
#define TR_VMS_F_TP	4	/* VMS F.TAPE format */
#define TR_VMS_F_TR	5	/* VMS F.TR (transparent) format */

#define NUM_F_TYPES	6

struct gen_ff			/* generic F format */
	{
	int padd;		/* is padding required? */
				/* padding is accepted on input.  It is */
				/* not produced on output */
	char pchar;		/* padd character */
	};

/*
 *	Define a structure that is to be filled with limits and defaults
 *	for record lengths and block sizes.
 *	The data associated with this structure is in fxrmisc.c
 */
struct f_rec_limit_s
	{
	int min_rsz;	/* minimum record size */
	int max_rsz;	/* maximum record size */
	int def_rsz;	/* default record size */
	int min_mbs;	/* minimum block size */
	int max_mbs;	/* maximum block size */
	int def_mbs;	/* default block size */
	};
#endif
