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

#pragma ident "@(#) libu/clib/sigctl.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <liberrno.h>

/*
 * SIGCTL has been called from both FORTRAN and PASCAL but the two
 * compilers generate code that calls SIGCTL that is not compatible.
 * Because of this SIGCTL may vary from site to site and we discourage
 * its use. Codes should be changed to call either FSIGCTL or PSIGCTL.
 * This will allow codes to be ported to other CRAY sites. The use
 * of SIGCTL may give different results at different sites.
 */

#define ACTENT(x,y)   #x,y,(sizeof(#x)-1)

static struct acts {
	char *aname;
	int anum;
	int alen;
} acts[] = {
	ACTENT(REGISTER, SCTL_REG),
	ACTENT(DUMPCORE, SCTL_DMP),
	ACTENT(KILL, SCTL_KIL),
	ACTENT(IGNORE, SCTL_IGN),
	ACTENT(STOP, SCTL_IGN),
	ACTENT(CONT, SCTL_CONT),
};

#define ACTCNT	sizeof(acts) / sizeof(struct acts)

_f_int
SIGCTL(action, num, func)
long	action;
long	*num;
void  *func;
{
#ifdef	_CRAY2
	/* Check if the first parameter is a character pointer. 	*/
	/* If so, then the old calling sequence was used, and we call	*/
	/* FSIGCTL.							*/
	if (_isfcd(action)) {
		fprintf(stderr,"%s%s%s%s",
		" WARNING:\n",
		" SIGCTL was called with the old calling sequence.\n",
		" This was accepted, but FSIGCTL should be used if this\n",
		" calling sequence is desired.\n\n");
		return(FSIGCTL(action, num, func));
	}
	else
#endif
		return(sigctl(*(long *)action, *num, (void (*)())func));
		
}

/*
 * FSIGCTL is the FORTRAN callable link to 'sigctl'
 * It provides the usual 'C' type interface, and
 * an interface using strings.
 */

#ifndef _ADDR64
FSIGCTL(long action, long num,void *func)
#else
FSIGCTL(_fcd action, _fcd num, void *func)
#endif
{
#define SLEN 20
	int	signum, actnum, i;
	char	*s;
	char	str[SLEN];
	int isfcd;

	/*
 	 * Determine whether the action and signal are strings or
 	 * integers.
 	 */

#ifdef _ADDR64
	if (_numargs()* sizeof(long) != 2*sizeof(_fcd) + sizeof(long))
		_lerror(_LELVL_ABORT,FEARGLST,"FSIGCTL");
	isfcd = 1;
#else
	isfcd = _isfcd(action);
#endif
	if (isfcd) {
		_fcd	fcdact;

		fcdact	= *(_fcd *) &action;
		s	= _fcdtocp(fcdact);

		for (i = 0; i < ACTCNT; i++) {
			if (strncmp(acts[i].aname, s, acts[i].alen) == 0) {
				actnum = acts[i].anum;
				break;
			}
		}
		if (i == ACTCNT)
			return(-1L);
	} 
#ifndef _ADDR64
		else
			actnum = *(long *)action;
#endif
#ifdef _ADDR64
	isfcd = 1;
#else
	isfcd = _isfcd(num);
#endif

	if (isfcd) {
		_fcd	fcdnum;

		fcdnum	= *(_fcd *) &num;

		if ((s = _f2ccpy(fcdnum, str, SLEN)) == NULL)
			return(-1L);

		if (str2sig(s, &signum) == -1)
			return(-1L);
	} 
#ifndef _ADDR64
	else
		signum = *(long *)num;
#endif

	return(sigctl(actnum, signum, (void (*)())func));
}

#ifndef _ADDR64
/* PSIGCTL is the PASCAL callable link to 'sigctl' */
PSIGCTL(action, num, func)
int	*action, *num;
void **func;
{
	/*
	 * Pascal calling sequence passes a pointer to the address
	 * of the function.
	 */
	return(sigctl(*action, *num, (void (*)())*func));
}
#endif
