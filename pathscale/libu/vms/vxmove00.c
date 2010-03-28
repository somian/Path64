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


#pragma ident "@(#) libu/vms/vxmove00.c	92.1	06/23/99 13:56:22"
#include <string.h>

/*
 *	ISTAT = VXMOVE00(SOURCE,ISB,NUM,DEST,IDB)
 *
 *	Parameters:
 *
 *		SOURCE	Word address of source string.
 *
 *		ISB	Address of source byte offset.  Byte 1 is the
 *			first byte of SOURCE.
 *
 *		NUM	Address of byte count.
 *
 *		DEST	Word address of destination string.
 *
 *		IDB	Address of destination byte offset.  Byte 1 is
 *			the first byte of DEST.
 *
 *	ISTAT:
 *
 *		-1	if called with less than 5 arguments, if
 *			NUM < 0, if ISB < 1, or if IDB < 1.
 *
 *		0	if data moved
 */

long
VXMOVE00(
	long	*source,	/* Address of source data	*/
	long	*isb,		/* Starting byte of source data	*/
	long	*num,		/* Number of bytes to move	*/
	long	*dest,		/* Address of destination data	*/
	long	*idb		/* First byte of destination	*/
	)
{
	char	*from, *to;
	int	count;
	long	status;

	status	= -1;

	if (_numargs() >= 5 && *num >= 0 && *isb > 0 && *idb > 0) {

		from	= (char *)source + (*isb - 1);
		to	= (char *)  dest + (*idb - 1);
		count	= *num;

		(void) memcpy(to, from, count);

		status	= 0;
	}

	return(status);
}
