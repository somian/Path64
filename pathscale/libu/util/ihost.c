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


#pragma ident "@(#) libu/util/ihost.c	92.1	07/07/99 13:18:33"


#include <sys/target.h>

/*
 *	Get information about the host system from libc variables
 *	set up in $START.
 */

extern struct target __target;

int
IHOST_CLKPICO@()
{
	return (__target.mc_clk);	/* system clock time in picoseconds */
}

IHOST_BANKS@()
{
	return (__target.mc_bank);	/* number of memory banks */
}

IHOST_PMT@()
{
	return (__target.mc_pmt);	/* Primary Machine Type */
}

