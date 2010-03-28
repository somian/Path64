/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#pragma ident "@(#) libf/pxf/pxfgetarg.c	92.2	09/15/99 10:41:12"

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <string.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

#define	BLANK	((int) ' ')

#ifdef _UNICOS
extern	int	_argc;		/* Number of argv fields */
extern	char	**_argv;	/* Pointer to pointer to arguments */
#elif defined(__mips)
extern  int     __Argc;         /* Number of argv fields */
#pragma weak    __Argc
extern  char    **__Argv;       /* Pointer to pointer to arguments */
#pragma weak    __Argv
#elif defined(__linux)
extern	int	f__xargc;	/* Number of argv fields */
extern	char	**f__xargv;	/* Pointer to pointer to arguments */
#elif defined(BUILD_OS_DARWIN)
extern int NXArgc;
extern char **NXArgv;
#else
extern  int     __xargc;         /* Number of argv fields */
extern  char    **__xargv;       /* Pointer to pointer to arguments */
#endif

#ifdef _UNICOS
void
PXFGETARG(
#else
void
_PXFGETARG(
#endif
	_f_int	*M,		/* Command line argument number */
	_fcd	BUF,		/* Character variable to receive argument */
	_f_int	*ILEN,		/* Significant length of argument */
	_f_int	*IERROR		/* Error status */
)
{
	int	arglen, argnum, errsts, movlen, varlen;
	char	*argstr, *varstr;

	errsts	= 0;
	arglen	= 0;
	argnum	= *M;

	varlen	= _fcdlen (BUF);
	varstr	= _fcdtocp(BUF);

	/* Check if argument exists */

#ifdef _UNICOS
	if (argnum < 0 || argnum >= _argc) {
#elif defined(__mips)
	if (argnum < 0 || argnum >= __Argc) {
#elif defined(__linux)
	if (argnum < 0 || argnum >= f__xargc) {
#elif defined(BUILD_OS_DARWIN)
	if (argnum < 0 || argnum >= NXArgc) {
#else
	if (argnum < 0 || argnum >= __xargc) {
#endif
		errsts	= EINVAL;
		arglen	= 0;
		movlen	= 0;
	}
	else {
#ifdef _UNICOS
		argstr	= _argv[argnum];
#elif defined(__mips)
		argstr	= __Argv[argnum];
#elif defined(__linux)
		argstr	= f__xargv[argnum];
#elif defined(BUILD_OS_DARWIN)
		argstr	= NXArgv[argnum];
#else
		argstr	= __xargv[argnum];
#endif
		arglen	= strlen(argstr);
		movlen	= arglen;
	}

	/*
	 * If argument is longer than the user variable,
	 * then it must be truncated.
	 */

	if (arglen > varlen) {
		errsts	= ETRUNC;
		movlen	= varlen;
	}

	/* Copy argument to user variable */

	if (movlen > 0)
		(void) memcpy(varstr, argstr, movlen);

	/* Blank-fill user variable, if necessary */

	if (varlen > movlen)
		(void) memset(varstr + movlen, BLANK, varlen - movlen);

	*IERROR	= errsts;
	*ILEN	= arglen;

	return;
}

#ifndef _UNICOS

void
pxfgetarg_(
	_f_int  *M,             /* Command line argument number */
	char    *BUF,           /* Character var to receive argument */
	_f_int  *ILEN,          /* Significant length of argument */
	_f_int  *IERROR,        /* Error status */
	int     len1
)
{
	_PXFGETARG( M, _cptofcd(BUF, len1), ILEN, IERROR);
	return;
}

#endif /* not _UNICOS */
