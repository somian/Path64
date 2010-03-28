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


#pragma ident "@(#) libu/trbk/c1/printvec.c	92.1	07/01/99 13:48:28"
#include  <stdio.h>

/*
 *	_printvec	Prints a formatted print of the contents of the
 *			requested vector registers.
 *
 *	Arguments:
 *		unit	Output stream (e.g., stdout, stderr)
 *		regv	Array containing all of the vector register
 *			contents (64 bits)
 *		regfrst	First vector register to display (0 to 7)
 *		reglast	Last vector register to display (regfrst to 7)
 *		veclen	Contents of the VL register
 */
void
_printvec(
	FILE	*unit,
	long	regv[],
	long	regfirst,
	long	reglast,
	long	veclen
)
{
	register short	i, vl, indx;

	if (veclen == 0)
		vl	= _MAXVL - 1;
	else
		vl	= veclen - 1;

	indx	= 0;

	for (i = regfirst; i <= reglast; ++i) {
		register short	j, repeat;
		register long	sreg, vreg;
		char		string[20];

		for (j = 0; j < _MAXVL; ++j) {

			vreg	= regv[indx++];

			if (j == 0 || j == vl || j == _MAXVL-1 || vreg != sreg) {

				sreg	= vreg;

				(void) _interpret(vreg, string);

				fprintf(unit, " V%d(%3d): %022o %s ", i, j, vreg, string);

				repeat	= 1;

				if (j == vl)
					fprintf(unit, "  (VL: %d)\n", vl + 1);
				else
					putc('\n', unit);

			}
			else
				if (repeat) {
					fprintf(unit, "             (Same as above)\n");
					repeat	= 0;
				}
		}

		putc('\n', unit);

	}

	return;
}
