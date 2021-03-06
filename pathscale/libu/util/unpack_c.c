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


#pragma ident "@(#) libu/util/unpack_c.c	92.1	07/07/99 13:18:33"

#include <fortran.h>
#include <liberrno.h>

/*
 *	UNPACK	Unpack full 64-bit words into partial words
 *
 *	UNPACK takes 64-bit words and extracts 1-, 2-, 4-, 8-,
 *	16-, or 32-bit quantities and places them right-justified
 *	into 64-bit words.  The quantities will optionally be
 *	sign-extended.
 *
 *	CALL UNPACK ( P, NBITS, U, COUNT [,SEF] )
 *
 *	On entry:
 *
 *	NBITS	Size of quantities to extract from each
 *		packed word.  NBITS must be a power of two
 *		(1, 2, 4, 8, 16, or 32).
 *
 *	P	Vector of 64-bit words to be unpacked
 *
 *	COUNT	Number of partial word quantities to unpack
 *
 *	SEF	Optional sign extension flag.  If .TRUE.,
 *		items will be sign extended.  If omitted or
 *		.FALSE., items will not be sign extended.
 *
 *	On exit:
 *
 *	U	Vector of unpacked data
 *
 *	(COUNT * NBITS) need not be a full-word multiple (i.e.,
 *	a multiple of 64).
 */

void
UNPACK(
	_f_int	*p,
	_f_int	*nbits,
	_f_int	*u,
	_f_int	*count,
	_f_log	*sef
)
{
	register short	sefl;		/* Sign extension flag */
	register int	nb;
	register int	ni;

#ifdef	_UNICOS
	if (_numargs() < 4)
		_lerror(_LELVL_ABORT, FEPCKARG);

	sefl	= (_numargs() > 4) ? _lvtob(*sef) : 0;
#else
	sefl	= _lvtob(*sef);
#endif

	nb	= *nbits;
	ni	= *count;

	if (nb < 0)
		_lerror(_LELVL_ABORT, FEPCKNEG);

	if (nb == 0)
		_lerror(_LELVL_ABORT, FEPCKPW2);

	if (ni > 0) {
		register short	cpw;	/* Chunks per word */
		register short	remr;	/* Remainder */
		register int	i;
		register int	items;	/* Number of full-word items */
		register long	mask;	/* Mask for each item */
		register long	sehw;	/* Sign extension helper word */
		register unsigned long	word;	/* Scratch word */

		cpw	= 64 / nb;		/* Chunks per word */
		items	= (ni * nb) >> 6;	/* (count * nbits) / 64 */
		remr	= (ni * nb) & 077;	/* Remainder */
		mask	= (1 << nb) - 1;
		sehw	= sefl << (nb - 1);

		switch (nb) {

			case 32:
#pragma _CRI ivdep
				for (i = 0; i < items; i++) {
					word	= *p++;
					*u++	= ((word >> 32) ^ sehw) - sehw;
					*u++	= ((word & mask) ^ sehw) - sehw;
				}
				break;

			case 16:
#pragma _CRI ivdep
				for (i = 0; i < items; i++) {
					word	= *p++;
					*u++	= ((word >> 48) ^ sehw) - sehw;
					*u++	= (((word >> 32) & mask) ^ sehw) - sehw;
					*u++	= (((word >> 16) & mask) ^ sehw) - sehw;
					*u++	= ((word & mask) ^ sehw) - sehw;
				}
				break;

			case 8:
#pragma _CRI ivdep
				for (i = 0; i < items; i++) {
					word	= *p++;
					*u++	= ((word >> 56) ^ sehw) - sehw;
					*u++	= (((word >> 48) & mask) ^ sehw) - sehw;
					*u++	= (((word >> 40) & mask) ^ sehw) - sehw;
					*u++	= (((word >> 32) & mask) ^ sehw) - sehw;
					*u++	= (((word >> 24) & mask) ^ sehw) - sehw;
					*u++	= (((word >> 16) & mask) ^ sehw) - sehw;
					*u++	= (((word >>  8) & mask) ^ sehw) - sehw;
					*u++	= ((word & mask) ^ sehw) - sehw;
				}
				break;

			case 4:
			case 2:
			case 1:
				for (i = 0; i < items; i++) {
					register short	shft;
					register short	j;

					word	= *p++;
					shft	= 64 - nb;

#pragma _CRI shortloop
					for (j = 0; j < cpw; j++) {
						*u++	= (((word >> shft) & mask) ^ sehw) - sehw;
						shft	= shft - nb;
					}
				}
				break;

			default:
				_lerror(_LELVL_ABORT, FEPCKPW2);
				break;

		} /* switch */

		if (remr > 0) {
			register short	shft;
			register short	j;

			word	= *p;
			shft	= 64 - nb;

#pragma _CRI novector
			for (j = remr; j > 0; j = j - nb) {
				*u++	= (((word >> shft) & mask) ^ sehw) - sehw;
				shft	= shft - nb;
			}

		}
	}

	return;
}
