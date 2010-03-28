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


#pragma ident "@(#) libu/util/mpp/_unpack.c	92.1	07/07/99 13:23:08"

/*
 *	_unpack		C callable subroutine to unpack bytes from a buffer.
 *			Unpacking terminates if maximum byte count is reached
 *			or if the terminating byte is encountered, whichever
 *			occurs first.  The terminating byte, if specified and
 *			if encountered, is not unpacked.
 *
 *	Returns:
 *
 *		>= 0	Number of bytes unpacked.
 *		  -1    Argument error detected.
 *
 *		No processing takes place and -1 is returned if this routine
 *		is called with mb < 0, called with less than 3 arguments, or
 *		called with an invalid terminating character.
 *
 *	Notes:
 *
 *		The tc argument is required on SPARC systems.
 */

long
_unpack(
	char	*cp,	/* Packed buffer (8-bit bytes) */
	long	*up,	/* Word buffer to receive unpacked data */
	long	bc,	/* Maximum number of bytes to unpack */
	int	tc)	/* (optional argument) Character to terminate unpacking
			 * (omitted or -1 if no terminating character).  Valid 
			 * terminating characters are in the range 
			 * 0 <= tc <= O'177. */
{
	int i;
	int narg;

#ifdef	_CRAY
	narg = _numargs();
#else
	narg = 4;		/* assume all arguments are passed */
#endif

	if (narg < 3)
		return(-1);

	if (narg > 3 && tc != -1) {

		if (tc < 0 || tc > 0177)
			return(-1);

		for (i=0; i<bc; i++) {
			if (cp[i] == tc)
				break;
			up[i] = cp[i];	
		}	
	}
	else {
		for (i=0; i<bc; i++)
			up[i] = cp[i];	
	}	

	return(i);
}
