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


#ifndef __BIT_H__
#define __BIT_H__
/*  $Header$ */
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	actual or intended publication of such source code.	*/

#include <cmplrs/host.h>

#define		NBB		8	/* # bits in byte	*/
#define		NBSI		16	/* # bits in short int	*/
#define		NBI		32	/* # bits in int	*/
#define		NBLL		64	/* # bits in long long	*/

extern int32 F77mask[], *F77zmask;
extern int64 F77llmask[], *F77llzmask;
#endif /* !__BIT_H__ */
