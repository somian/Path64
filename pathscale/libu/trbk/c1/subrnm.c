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


#pragma ident "@(#) libu/trbk/c1/subrnm.c	92.1	07/01/99 13:48:28"
#include <string.h>
#include <cray/stk.h>

#define  BTOW(x)   (((x)+7)>>3)
#define  EOS	   '\0'
#define  SUCCESS   0
#define  FAILURE   1
#define  MAX_ENT_LEN 32

/*
 *	Return "subname" as the name to which "fpptr" refers.
 *	"fpptr" is the pointer to the caller's stack frame.
 *	If "fpptr" is in range, _subrnm() returns SUCCESS(0),
 *	otherwise it returns FAILURE(1).
 */

long 
_subrnm(fpptr, subname, baseaddr, argcnt, lineno, la)
struct stack_frame *fpptr;
char subname[];
long *baseaddr, *argcnt, *lineno;
long la;
{
	struct TNB *tnb;
	struct arlst *arlst;
	int nchars;
	char *ptr;

	if (la == 0)
		la = sbreak(0);
	if ((long)fpptr < 4 || (long)fpptr >= la) {
		subname[0] = EOS;
		return (FAILURE);
	}
	tnb = fpptr->tnb;
	*baseaddr = (long)tnb->entry;
	arlst = (struct arlst *)fpptr->prevargs;
	*argcnt = arlst->nargs;
	*lineno = arlst->lineno;
	nchars = tnb->nmlen;
	ptr = (char *)((long *)tnb - BTOW(nchars));
	if (nchars > MAX_ENT_LEN - 1)
		nchars = MAX_ENT_LEN - 1;
	(void) strncpy(subname, ptr, nchars);
	subname[nchars] = EOS;
	return (SUCCESS);
}
