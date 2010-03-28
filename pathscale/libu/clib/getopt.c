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

#pragma ident "@(#) libu/clib/getopt.c	92.1	07/01/99 13:42:20"
#include <fortran.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <liberrno.h>

extern	char	**_argv;
extern	int	_argc;

#define EOF	(-1)

#define ERR(s, c)	if (opterr) {	\
	char errbuf[2];			\
	errbuf[0]	= c;		\
	errbuf[1]	= '\n';		\
	(void) write(fileno(stderr), _argv[0], (unsigned)strlen(_argv[0]));\
	(void) write(fileno(stderr), s, (unsigned int) strlen(s));	\
	(void) write(fileno(stderr), errbuf, 2);	\
}


extern int	opterr;
extern int	optind;
extern int	optopt;
extern char	*optarg;

static _f_int
_getoptc(char *opts, int loptlen, char *carg, int length, char ch);

static _f_int
_getvarg(char *carg, int length, char ch) ;

static int	c;

static void
mystrncpy(p1, p2, n, pad)
register char *p1, *p2;
register int n;
register char pad;
{
	while (n-- > 0 && (*p1 = *p2++))
		++p1;

	if (!*p1)
		*p1	= pad;

	if (n < 0) {
		ERR(": argument too long -- ", c);
		c	= '?';
	}
	else
		while (n-- > 0)
			*++p1	= pad;

	return;
}

#ifndef _ADDR64
_f_int
GETOPT(lopts, arg, argsz)
long	lopts;
long	arg;
long	*argsz;
{
	register char *cp;
	register char *opts;
	char	ch;
	register length;
	long loptlen;
	register char *carg;

	if (_numargs() < 2)
		return( (_f_int) -1);

	if (_isfcd(lopts)) {		/* If Fortran character descriptor */
		_fcd	flopts;

		flopts	= *(_fcd *) &lopts;
		opts	= _fcdtocp(flopts);
		loptlen	= _fcdlen (flopts);
	}
	else {				/* Hollerith */
		opts	= (char *) lopts;
		loptlen	= strlen(opts) + 1;
	}

	if (_isfcd(arg)) {		/* If Fortran character descriptor */
		_fcd	farg;

		farg	= *(_fcd *) &arg;
		carg	= _fcdtocp(farg);
		length	= _fcdlen (farg);
		ch	= ' ';
	}
	else {				/* Hollerith */
		if (_numargs() < 3)
			return( (_f_int) -1);

		carg	= (char *) arg;
		length	= *argsz * sizeof(long);
		((long *)arg)[*argsz-1]	= 0;
		ch	= '\0';
	}
	return(_getoptc(opts, loptlen, carg, length, ch));
}

_f_int
GETVARG(arg, argsz)
long	arg;
long	*argsz;
{
	int	length;
	char	*carg;
	char ch;

	if (_numargs() < 1)
		return( (_f_int) -1);

	if (_isfcd(arg)) {		/* If Fortran character descriptor */
		_fcd	farg;

		farg	= *(_fcd *) &arg;
		carg	= _fcdtocp(farg);
		length	= _fcdlen (farg);
		ch = ' ';
	}
	else {				/* Hollerith */
		if (_numargs() < 2)
			return( (_f_int) -1);

		carg	= (char *) arg;
		length	= *argsz * sizeof(long);
		((long *)arg)[*argsz-1]	= 0;
		ch = '\0';
	}

	return(_getvarg(carg,length,ch));
}

_f_int
GETOARG(arg, argsz)
long	arg;
long	*argsz;
{
	char	*carg;
	char	ch;
	int	length;

	if (_numargs() < 1)
		return( (_f_int) -1);

	if (optind >= _argc)
		return(0);

	optarg	= _argv[optind++];

	if (_isfcd(arg)) {		/* If Fortran character descriptor */
		_fcd	farg;

		farg	= *(_fcd *) &arg;
		carg	= _fcdtocp(farg);
		length	= _fcdlen (farg);
		ch	= ' ';
	}
	else {				/* Hollerith */
		if (_numargs() < 2)
			return( (_f_int) -1);

		carg	= (char *) arg;
		length	= *argsz * sizeof(long);
		((long *)arg)[*argsz-1]	= 0;
		ch	= '\0';
	}

	mystrncpy(carg, optarg, length, ch);

	return(1);
}
#endif

static 
_f_int
_getvarg(char *carg, int length, char ch) 
{
	char	*p, *p2;
loop:
	if (optarg && *optarg) {
		while (*optarg == ',')
			++optarg;
		p	= optarg;
		while (*p && *p != ',')
			++p;
		if (*p)
			*p++	= '\0';
		if (*(p2 = optarg)) {

			optarg	= p;
			mystrncpy(carg, p2, length, ch);
			return(1);
		}
	}
	if (optind >= _argc || _argv[optind][0] == '-')
		return(0);
	optarg	= _argv[optind++];
	goto loop;
}

static 
_f_int
_getoptc(char *opts, int loptlen, char *carg, int length, char ch)
{
static int sp = 1;
char *cp;

	if (sp == 1)
		if (optind >= _argc ||
		   _argv[optind][0] != '-' || _argv[optind][1] == '\0')
			return(EOF);
		else if (strcmp(_argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt	= _argv[optind][sp];
	c	= _argv[optind][sp];
	if (c == ';' || c == ':' || (cp = memchr(opts, c, loptlen)) == NULL) {
		ERR(": illegal option -- ", c);
		if (_argv[optind][++sp] == '\0') {
			optind++;
			sp	= 1;
		}
		optarg	= NULL;
		return('?');
	}
	if (*++cp == ':') {

		if (_argv[optind][sp+1] != '\0') {
			optarg	= &_argv[optind++][sp+1];
		} else if (++optind >= _argc) {
			ERR(": option requires an argument -- ", c);
			sp	= 1;
			optarg	= NULL;
			return('?');
		} else
			optarg	= _argv[optind++];

		sp	= 1;

		mystrncpy(carg, optarg, length, ch);

	} else if (*cp == ';') {
		optarg	= &_argv[optind++][sp+1];
		sp	= 1;
	} else {
		if (_argv[optind][++sp] == '\0') {
			sp	= 1;
			optind++;
		}
		optarg	= NULL;
	}
	return(c);
}


_f_int
GETOARGC(_fcd arg)
{

	if (_numargs() != sizeof(_fcd)/sizeof(long) )
		_lerror(_LELVL_ABORT,FEARGLST,"GETOARGC");

	if (optind >= _argc)
		return(0);

	optarg	= _argv[optind++];

	mystrncpy(_fcdtocp(arg), optarg, _fcdlen(arg), ' ');

	return(1);
}

_f_int
GETVARGC(_fcd arg)
{

	if (_numargs() != sizeof(_fcd)/sizeof(long) )
		_lerror(_LELVL_ABORT,FEARGLST,"GETVARGC");

	return(_getvarg(_fcdtocp(arg),_fcdlen(arg),' '));
}

_f_int
GETOPTC(_fcd lopts, _fcd arg)
{

	if (_numargs() != 2*sizeof(_fcd)/sizeof(long) )
		_lerror(_LELVL_ABORT,FEARGLST,"GETOPTC");

	return(_getoptc(_fcdtocp(lopts),_fcdlen(lopts),_fcdtocp(arg),_fcdlen(arg),' '));
}

