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


static char USMID[] = "@(#) libf/fio/c1/ibmerr.c	92.0	10/08/98 14:30:10";

#include <stdio.h>
#include <foreign.h>
#include "fio.h"

/*
 * This routine is called by WRITIBM under UNICOS 
 * to write a message to STDERR. It formats the 
 * current unit number into the message at the
 * specified position.
 */

void
_ibmerr(len, unitpos, ibmerrmsg)
int len;	/* length of message */
int unitpos;	/* position for UNIT number */
char *ibmerrmsg; /* error message */
{
	int	unum;		/* unit number */
	int	i;
	int	pr, d;
	char	*unit_pos;	/* pointer into message */
	FIOSPTR fiosp;

	GET_FIOS_PTR(fiosp);
	unum = fiosp->f_curun;	/* find current unit number */

	unit_pos = ibmerrmsg+unitpos;

	/* Format unit number into message */

        pr = 0;
        for (i = 1000000; i != 1; i /= 10)
        {
                if ((pr |= (d = unum / i)))
                        *unit_pos++ = d + '0';
                unum %= i;
        }
        *unit_pos++ = unum + '0';

	/* Replace last characters of message with newline and null*/

	*(ibmerrmsg+len-2) = '\n';
	*(ibmerrmsg+len-1) = '\0';
        fprintf(stderr,"%s", ibmerrmsg);
}
