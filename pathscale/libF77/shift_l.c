/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* $Header: /home/bos/bk/kpro64-pending/libF77/shift_l.c 1.5 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	actual or intended publication of such source code.	*/

#include "bit.h"
#include "f77defs.h"

/* Logically shift m k-places. If k > NBI, m = undefined.
 * k > 0 => left shift.
 * k < 0 => right shift.
 * k = 0 => no shift.
 */

int32 shft_l(int32 *m, int32 *k)
{
	int32 l;
	short left;

	left = (*k > 0);
	l = left ? *k : -*k;

	if (l > 0 && l < NBI)
		if (left)
			return(*m << l);
		else
			return( (*m >> l) & F77mask[NBI - l] );
	else if (l == 0)
		return(*m);
	else return(0L);
}
