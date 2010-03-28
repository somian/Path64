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


#pragma ident "@(#) libu/trbk/mpp/who.c	92.1	07/01/99 13:49:30"
#include <string.h>
#include <unistd.h>
#include <cray/stk.h>

extern	struct DSIB	*_get_fp();	/* Returns frame pointer register */

/*
 *	_who_called_me(lineno, name, namelen, levels, addr)
 *
 *	This routine returns information about the caller of the routine
 *	that called _who_called_me().  For example, if A calls B and B
 *	calls _who_called_me(), this routines returns the name of A and
 *	the line number in A from where B was called.
 *
 *		lineno	Address to return line number
 *		name	Address to return name
 *		namelen	Maximum number of characters to return
 *		levels	Number of levels to go back
 *		addr	Address to return call address (optional)
 *
 *	This function returns the actual number of characters copied
 *	to name or -1 if error.
 */

int
_who_called_me(
	int	*lineno,	/* Line number of caller	*/
	char	*name,		/* Name of caller		*/
	int	namelen,	/* Maximum length of name	*/
	int	levels,		/* Number of levels to go back	*/
	int	*addr		/* Address of call (optional)	*/
)
{
	register int	i, len;
#ifndef	_CRAYT3D
	register int	ciwi;	/* ci word index */
#endif
	char		*snp;	/* Subprogram name pointer */
	struct DSIB	*fp;	/* Frame pointer */
	struct SSIB	*sbp;	/* SSIB pointer */

	len	= 0;
	*lineno	= 0;
	fp	= _get_fp() - 1;

	for (i = 0; i <= levels; i++) {

		/* Get lineno and addr at next-to-last level */

		if (i == levels) {

			if (_numargs() > 4)
				*addr	= (int) fp->ra - 4;
#ifdef	_CRAYT3D
			*lineno	= fp->ci.lineno;
#else
			ciwi	= fp->ciwi;
#endif
		}

		fp	= (struct DSIB *) fp->fp;	/* Explicit cast is temporary */

		if (fp == (struct DSIB *) NULL)
			goto done;

		/* Back up pointer to start of DSIB */

		fp	= fp - 1;
	}

	sbp	= fp->ssib;

	if (sbp != (struct SSIB *) NULL) {	/* If we have an SSIB */

#ifndef	_CRAYT3D
		if (ciwi != 0)		/* If we have a ci word */
			*lineno	= ((struct ci_word *) (fp->ssib))[ciwi].lineno;
#endif

		len	= sbp->namelen;
		snp	= (char *) sbp + sbp->SSIB_len;

		if (len > namelen)
			len	= namelen;

		(void) memcpy(name, snp, len);
	}

done:
	return (len);
}
