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


#ifndef _FTRACEBK_
#pragma ident "@(#) libu/trbk/c1/tracebk.c	92.1	07/01/99 13:48:28"
#endif
#if !defined(_FTRACEBK_) && __STDC__ && _RELEASE >= 3
#pragma _CRI duplicate _tracebk as tracebk
#endif

#include <stdio.h>
#include <fortran.h>
#include <malloc.h>
#include <cray/stk.h>

/* Define number of parcels in a return-jump instruction */

#if	_MAXVL > 64
#define RJPARCELS	3
#else
#define RJPARCELS	2
#endif

#define	MAX_ENT_LEN	32	/* Maximum entry point name length + 1 */

#define	SAVEXTND(STKLOC,VAL)
#define	RESTORXTND(STKLOC)

extern long	_subrnm();
extern long	__tracebk();

/*
 *  Print a traceback (on stdout (or optunit if supplied)),
 *  starting with my caller.  If "depth" argument is not supplied,
 *  25 is used.
 *
 *		 ( <0:  trace depth = 25
 *		 (  0:  Print 1-line "Where am I" message
 *	"depth" -(  1:  Print 1-line trace (caller's name/lineno)
 *		 (  2:  Print "Where am I" and 1-line trace
 *		 ( >2:  trace depth (25 if above 50)
 *
 */

int 
#ifdef _FTRACEBK_
TRACEBK(
	long	*depth,
	_fcd	filenm
#else
_tracebk(
	int	depth,
	FILE	*optunit
#endif
)
{
	register FILE 	*fp;
	register long	deep, tflag;
	register short	n1, n2, n3, n4;
	int		lineno, m1, m2;
	long		namerr, la, namlen;
	char		name[MAX_ENT_LEN];
	char		*namptr;
	static char	*sorry = " Traceback not possible, trace data corrupted!\n";
	static char	*whereami = " Currently executing";
	static char	*tracemsg = " Traceback initiated";
	struct stack_frame	*myid;
	struct stack_frame	*caller;
	struct arlst	*arlst;
	long		baseaddr;

	fp	= stdout;
	n1	= _numargs();

	if (n1 > 0) {
#ifdef	_FTRACEBK_
		deep	= *depth;

		if (n1 > 1) {

			namlen	= _fcdlen(filenm);

			if ((namptr = _f2ccpy(filenm)) == NULL)
				return(1);

			fp	= fopen(namptr, "a");

			free(namptr);

			if (fp == NULL)
				return(1);
		}
#else
		deep	= depth;

		if (n1 > 1)
			fp	= optunit;
#endif
		tflag	= deep;
	}
	else {
		deep	= 25;
		tflag	= 3;
	}

	if (deep < 0 || deep > 50) {
		deep	= 25;
		tflag	= 3;
	}

	if (deep < 3)
		deep	= 25;

	if (tflag < 3)
		deep	= 2;

	la	= (long) _sbreak(0);

	SAVEXTND(savextnd, 0);	/* Turn off stack expand-in-place */

	myid	= (struct stack_frame *) GETB02@();
	arlst	= (struct arlst *) GETB01@();
	lineno	= arlst->lineno;
	caller	= myid->prev;

	if (myid == 0)
		namerr	= 1;
	else {
		namerr	= 0;
		namerr += _subrnm(myid, name, &baseaddr, &m1, &m2, la);
		namerr += _subrnm(caller, name, &baseaddr, &m1, &m2, la);
	}

	if (namerr) {
		(void) _write(fileno(fp), sorry, sizeof(sorry));
		return(0);
	}

	n3	= (long) myid->retn - RJPARCELS;
	n4	= baseaddr;
	n1	= (n3 >> 2);
	n2	= (n3 & 3) + 'a';
	n4	= n3 - n4;
	n3	= (n4 >> 2);
	n4	= (n4 & 3) + 'a';

	if (tflag != 1) {
		fprintf(fp, "\n %s at \"%s\"+0%o%c (P = 0%o%c, line %d)\n",
			tflag < 1 ? whereami : tracemsg, name,
			n3, n4, n1, n2, lineno);
	}

	if (tflag != 2)
		putc('\n', fp);

	n1	= 0;

	if (tflag) {
		n1	= __tracebk(fp, myid, caller, la, deep, 0, -1, 1);
		putc('\n', fp);
	}

	RESTORXTND(savextnd);	/* Restore stack expansion state */

#ifdef _FTRACEBK_
	if (n1 > 1)
		(void) fclose(fp);
#endif

	return(n1 ? 1 : 0);
}
