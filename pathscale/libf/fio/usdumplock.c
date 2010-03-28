/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/usdumplock.c	92.1	06/18/99 18:38:26"

#include <sys/types.h>
#ifndef KEY
#include <ulocks.h> 
#endif
#include <stdio.h>
#include <stdlib.h>
#include "fio.h"
#include <string.h>

#ifdef KEY
typedef void *ulock_t;	/* added in place of including ulocks.h */
#endif

extern int32 __usdumplock_f90(ulock_t *l, int32 *u, char *str, int32 len);
extern _f_int usdumplockf90_(ulock_t *l, _f_int *u, char *str, int len);
extern _f_int4 usdumplockf90_4_8_4_(ulock_t *l, _f_int4 *u, char *str, int len);
extern _f_int8 usdumplockf90_8_(ulock_t *l, _f_int8 *u, char *str, int len);

int32
__usdumplock_f90(ulock_t *l, int32 *u, char *str, int32 len)
{
	return usdumplockf90_(l, u, str, len);
}

_f_int
usdumplockf90_(ulock_t *l, _f_int *u, char *str, int len)
{
	char	*buff;
	unit	*cup;
	FILE	*fd;
	unum_t	unum;

	unum	= *u;
	cup	= _get_cup(unum);
	if (unum < 0 || !cup)
		return((errno=FEIVUNIT));

	if (cup->ufmt == NO && cup->useq == NO) {

		/* not formatted and not sequential */
		errno=FEFMTTIV;

		/*
	 	 * A formatted read or write is not allowed on an
		 * unformatted file.  We really want to say operation
		 * only allowed on formatted sequential files
		 */
		return(-1);
	}

/*	check to see if write is allowed on file. */
	if (!cup->ok_wr_dir_unf) {

		/* write not allowed. */
		errno	= FENOWRIT;
		return(-1);
	}
	if (cup->ufs != STD) {
		/* not an "stdio" file */
		errno	= FDC_ERR_NOSTRM; /* NOT the right msg - DLAI */
		return(-1);
	}
/*	get the FILE */
	fd	= cup->ufp.std;
	buff	= (char *) malloc( len + 1 );
	strncpy( buff, str, len );
	buff[len]	= '\0';
	usdumplock( *l, fd, buff );
	free( buff );
	return(0);
}

_f_int4
usdumplockf90_4_8_4_(ulock_t *l, _f_int *u, char *str, int len)
{
	_f_int8		unum = *u;
	return (_f_int4)usdumplockf90_8_(l,&unum,str,len);
}

_f_int8
usdumplockf90_8_(ulock_t *l, _f_int8 *u, char *str, int len)
{
	char	*buff;
	unit	*cup;
	FILE	*fd;
	unum_t	unum;

	unum	= *u;
	cup	= _get_cup(unum);
	if (unum < 0 || !cup)
		return((errno=FEIVUNIT));

	if (cup->ufmt == NO && cup->useq == NO) {

		/* not formatted and not sequential */
		errno=FEFMTTIV;

		/*
	 	 * A formatted read or write is not allowed on an
		 * unformatted file.  We really want to say operation
		 * only allowed on formatted sequential files
		 */
		return(-1);
	}

/*	check to see if write is allowed on file. */
	if (!cup->ok_wr_dir_unf) {

		/* write not allowed. */
		errno	= FENOWRIT;
		return(-1);
	}
	if (cup->ufs != STD) {
		/* not an "stdio" file */
		errno	= FDC_ERR_NOSTRM; /* NOT the right msg - DLAI */
		return(-1);
	}
/*	get the FILE */
	fd	= cup->ufp.std;
	buff	= (char *) malloc( len + 1 );
	strncpy( buff, str, len );
	buff[len]	= '\0';
	usdumplock( *l, fd, buff );
	free( buff );
	return(0);
}
