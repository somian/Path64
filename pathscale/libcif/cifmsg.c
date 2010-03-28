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


static char USMID[] = "@(#) libcif/cifmsg.c	30.3	07/26/96 07:19:13";


/*
 * Cif_Msginsert accepts a cif message structure and message text string.  It
 * returns a new text string with the message arguments inserted in place of
 * "%" printf fields in the old string.
 */

#define CIF_VERSION 3

#ifdef _ABSOFT
#include "cif.h"
#else
#include <cif.h>
#endif

#include <stdio.h>
#include <string.h>

#include "cif_int.h"

int Cif_Msginsert
#ifdef __STDC__
(char *msgtext, struct Cif_generic *cr, char *newtext, int ntlen)
#else
(msgtext, cr, newtext, ntlen)
char *msgtext;							/* pointer to original message text */	
struct Cif_generic *cr;				/* pointer to cif message structure */
char *newtext;							/* pointer to new message text buffer*/
int ntlen;								/* length of 'newtext' */
#endif
{
	int i, ocptr, argno, nargs;
	char ichr, **args;
	char *fchars = "dgiopuxXfeEgGcsn";

	for (ocptr = 0; ocptr < ntlen; ocptr++) newtext[ocptr] = '\0';
	if (cr->rectype == CIF_MESSAGE) {
		if (_cif_version <= 2) { /* must use v1,2 cif records */
			nargs = CIFMSG1(cr)->nargs;
			args = CIFMSG1(cr)->args;
		}
		else {
			nargs = CIFMSG(cr)->nargs;
			args = CIFMSG(cr)->args;
		}
	}
	else if (cr->rectype == CIF_ND_MSG) {
		nargs = CIFNMSG(cr)->nargs;
		args = CIFNMSG(cr)->args;
	}
	else if (cr->rectype == CIF_C_MESSAGE) {
	  	if (_cif_version == 1) { /* must use v1 cif records */

			nargs = CIFCMSG1(cr)->nargs;
			args = CIFCMSG1(cr)->args;

		}
		else { /* version 2 cif */

			nargs = CIFCMSG(cr)->nargs;
			args = CIFCMSG(cr)->args;

		}
	}
	else
		return (CIF_BADREQ);
	
	argno = ocptr = 0;
	while (ichr = *msgtext++) {
		if (ichr != '%') {
			if (ocptr < ntlen-1) newtext[ocptr++] = ichr;
		}
		else {
			ichr = *msgtext++;
			if (ichr == '%') {
				if (ocptr < ntlen-1) newtext[ocptr++] = ichr;
			}
			else {
				while (strchr (fchars, ichr) == NULL)
					ichr = *msgtext++;
				if (argno >= nargs) {
				  /* something has gone wrong, but do the best we
				     can, so that the user gets some message */
				  (void) strncpy (&newtext[ocptr],"?",1);
				  ocptr += 1;
				  continue;
				  /* return (CIF_BADREQ); gave in previously */
				}
				i = strlen (args[argno]);
				if (ntlen-ocptr-1 < i) i = ntlen-ocptr-1;
				(void) strncpy (&newtext[ocptr], args[argno++], i);
				ocptr += i;
			}
		}
	}
	return (0);
}
