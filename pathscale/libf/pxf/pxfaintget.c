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


#pragma ident "@(#) libf/pxf/pxfaintget.c	92.2	06/29/99 11:36:06"

/*
 *  PXFAINTGET -- Access an array of integers in the termios structure
 *  PXFAINTSET -- Access an array of integers in the termios structure
 *  PXFEINTGET -- Access an integer array element in the termios structure
 *  PXFEINTSET -- Access an integer array element in the termios structure
 *             (section 7.1.2.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFAINTGET(jtermios, compnam, iavalue, ialen, ierror)
 *     INTEGER jtermios, ialen, iavalue(ialen), ierror
 *     CHARACTER*(*) compnam
 *
 *     SUBROUTINE PXFAINTSET(jtermios, compnam, iavalue, ialen, ierror)
 *     INTEGER jtermios, ialen, iavalue(ialen), ierror
 *     CHARACTER*(*) compnam
 *
 *     SUBROUTINE PXFEINTGET(jtermios, compnam, ivalue, index, ierror)
 *     INTEGER jtermios, ivalue, index, ierror
 *     CHARACTER*(*) compnam
 *
 *     SUBROUTINE PXFEINTSET(jtermios, compnam, ivalue, index, ierror)
 *     INTEGER jtermios, ivalue, index, ierror
 *     CHARACTER*(*) compnam
 *
 *  Description:
 *
 *  PXFAINTGET accesses an array of integers in a structure.
 *  PXFAINTSET accesses an array of integers in a structure.
 *  PXFEINTGET accesses an integer array element within an array.
 *  PXFEINTSET accesses an integer array element within an array.
 *
 *  The arguments are:
 *
 *      jtermios -  default integer input variable containing an
 *                  input handle variable created with PXFSTRUCTCREATE.
 *      compnam  -  intput character variable or array element
 *                  containing the desired structure component name.
 *      index    -  default input integer variable specifying the
 *                  desired element in the array.
 *      ivalue   -  default integer variable containing the value of
 *                  the array element.  This is an input argument for
 *                  the SET entry points and an output argument for
 *                  the GET entry points.
 *      iavalue   - default integer array containing the values of
 *                  the array elements.  This is an input argument
 *                  for the SET entry points and an output argument
 *                  for the GET entry points.
 *      ialen    -  default input integer variable containing the
 *                  number of array elements to access.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFEINTGET, PXFEINTSET, PXFAINTGET, and PXFAINTSET may return
 *   one of the following error values:
 *
 *   EBADHANDLE If jhandle is invalid.
 *
 *   EINVAL     If index is out of range.
 *
 *   ENOMEM     If there is insufficient memory to create data
 *              structures needed by the routine.
 *
 *   ENONAM     If the component name is not defined for this
 *              structure.
 *
 *   EARRAYLEN  If the number of array elements to be returned
 *              exceeds ialen for the GET routines and only the
 *              first elements of iavalue have been set.  If the
 *              number of array elements to be set exceeds ialen for
 *              the SET routines and only the first elements of
 *              the structure array have been set.
 *              
 */

#include <errno.h>
#include <fortran.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/termios.h>
#include "pxfstruct.h"
#include "table.h"

/*
 * if index is < zero or > NCCS, (i.e., c_cc[NCCS]), return EINVAL.
 */
#define GETIFMATCHE(_A_,_B_,_C_) if (strcmp(_A_,#_C_) == 0) {\
	if(cindex >= 0 && cindex < arrlen) {\
		*ivalue = ((struct _B_ *)(pxfhand.pxfstructptr))->_C_[cindex];\
	} else {\
		errsts = EINVAL;\
	}\
}

#define SETIFMATCHE(_A_,_B_,_C_) if (strcmp(_A_,#_C_) == 0) {\
	if(cindex >= 0 && cindex < arrlen) {\
		((struct _B_ *)(pxfhand.pxfstructptr))->_C_[cindex] = *ivalue;\
	} else {\
		errsts = EINVAL;\
	}\
}

/*
 * if length in argument ialen does not match NCCS from c_cc[NCCS],
 * return error EARRAYLEN and:
 * 1. if ialen is less than zero, do not move anything.
 * 2. if ialen is greater than NCCS, move only NCCS elements.
 * 3. if ialen is less than NCCS, move only ialen elements.
 */
#define GETIFMATCHA(_A_,_B_,_C_) if (strcmp(_A_,#_C_) == 0) {\
	if(elemnum < 0 ) {\
		errsts = EARRAYLEN;\
	} else {\
		if(elemnum > arrlen) {\
			errsts = EARRAYLEN;\
			elemnum = NCCS;\
		} else {\
			if(elemnum < arrlen) {\
				errsts = EARRAYLEN;\
			}\
		}\
		for(i=0; i <= elemnum; i++) {\
			iavalue[i] = ((struct _B_ *)(pxfhand.pxfstructptr))->_C_[i];\
		} \
	} \
}

#define SETIFMATCHA(_A_,_B_,_C_) if (strcmp(_A_,#_C_) == 0) {\
	if(elemnum < 0 ) {\
		errsts = EARRAYLEN;\
	} else {\
		if(elemnum > arrlen) {\
			errsts = EARRAYLEN;\
			elemnum = NCCS;\
		} else {\
			if(elemnum < arrlen) {\
				errsts = EARRAYLEN;\
			}\
		}\
		for(i=0; i <= elemnum; i++) {\
			((struct _B_ *)(pxfhand.pxfstructptr))->_C_[i] = iavalue[i]; \
		} \
	} \
}

extern char *_fc_acopy(_fcd f);

#ifdef _UNICOS
void
PXFEINTGET(
#else	/* _UNICOS */
void
_PXFEINTGET(
#endif	/* _UNICOS */
	_f_int	*jhandle,
	_fcd	compnam,
	_f_int	*ivalue,
	_f_int	*index,
	_f_int	*ierror)
{
	char	*component;
	_f_int	cindex;
	int	arrlen;
	int	errsts;
	struct	pxfhandle pxfhand;
	cindex	= *index - 1;
	component	= _fc_acopy(compnam);
	errsts	= 0;
	*ivalue	= 0;

	/* verify component name is present. */
	if (component == NULL) {
		*ierror	= ENOMEM;
		return;
	}

	pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *jhandle);
	/* verify jhandle is present. */
	if (pxfhand.pxfstructptr == NULL)
		errsts	= EBADHANDLE;
	else {
		switch (pxfhand.pxftype) {
			case PXF_TERMIOS:
				arrlen	= NCCS;
				GETIFMATCHE(component,termios,c_cc)
				else {
					errsts	= ENONAME;
				}
				break;
			default:
				errsts	= ENONAME;
				break;
		}
	}
	free(component);
	*ierror	= errsts;
	return;
}

#ifdef _UNICOS
void
PXFEINTSET(
#else	/* _UNICOS */
void
_PXFEINTSET(
#endif	/* _UNICOS */
	_f_int	*jhandle,
	_fcd	compnam,
	_f_int	*ivalue,
	_f_int	*index,
	_f_int	*ierror)
{
	char	*component;
	_f_int	cindex;
	int	arrlen;
	int	errsts;
	struct	pxfhandle pxfhand;
	cindex	= *index - 1;
	component	= _fc_acopy(compnam);
	errsts	= 0;

	/* verify component name is present. */
	if (component == NULL) {
		*ierror	= ENOMEM;
		return;
	}

	pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *jhandle);
	/* verify jhandle is present. */
	if (pxfhand.pxfstructptr == NULL)
		errsts	= EBADHANDLE;
	else {
		switch (pxfhand.pxftype) {
			case PXF_TERMIOS:
				arrlen	= NCCS;
				SETIFMATCHE(component,termios,c_cc)
				else {
					errsts	= ENONAME;
				}
				break;
			default:
				errsts	= ENONAME;
				break;
		}
	}
	free(component);
	*ierror	= errsts;
	return;
}

#ifdef _UNICOS
void
PXFAINTGET(
#else	/* _UNICOS */
void
_PXFAINTGET(
#endif	/* _UNICOS */
	_f_int	*jhandle,
	_fcd	compnam,
	_f_int	*iavalue,
	_f_int	*ialen,
	_f_int	*ierror)
{
	char	*component;
	_f_int	arrlen;
	_f_int	elemnum;
	_f_int	errsts;
	int	i;
	struct	pxfhandle pxfhand;
	component	= _fc_acopy(compnam);
	elemnum	= *ialen;
	errsts	= 0;

	/* verify component name is present. */
	if (component == NULL) {
		*ierror	= ENOMEM;
		return;
	}

	pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *jhandle);
	/* verify jhandle is present. */
	if (pxfhand.pxfstructptr == NULL)
		errsts	= EBADHANDLE;
	else {
		switch (pxfhand.pxftype) {
			case PXF_TERMIOS:
				arrlen	= NCCS;
				GETIFMATCHA(component,termios,c_cc)
				else {
					errsts	= ENONAME;
				}
				break;
			default:
				errsts	= ENONAME;
				break;
		}
	}
	free(component);
	*ierror = errsts;
	return;
}

#ifdef _UNICOS
void
PXFAINTSET(
#else	/* _UNICOS */
void
_PXFAINTSET(
#endif	/* _UNICOS */
	_f_int	*jhandle,
	_fcd	compnam,
	_f_int	*iavalue,
	_f_int	*ialen,
	_f_int	*ierror)
{
	char	*component;
	_f_int	arrlen;
	_f_int	elemnum;
	_f_int	errsts;
	int	i;
	struct	pxfhandle pxfhand;
	component	= _fc_acopy(compnam);
	elemnum	= *ialen;
	errsts	= 0;

	/* verify component name is present. */
	if (component == NULL) {
		*ierror	= ENOMEM;
		return;
	}

	pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *jhandle);
	/* verify jhandle is present. */
	if (pxfhand.pxfstructptr == NULL)
		errsts	= EBADHANDLE;
	else {
		switch (pxfhand.pxftype) {
			case PXF_TERMIOS:
				arrlen	= NCCS;
				SETIFMATCHA(component,termios,c_cc)
				else {
					errsts	= ENONAME;
				}
				break;
			default:
				errsts	= ENONAME;
				break;
		}
	}
	free(component);
	*ierror	= errsts;
	return;
}

#ifndef _UNICOS
void
pxfeintget_(
	_f_int	*jhandle,
	char	*compnam,
	_f_int	*ivalue,
	_f_int	*index,
	_f_int	*ierror,
	int	lencompnam)
{
	_PXFEINTGET(jhandle, _cptofcd(compnam, lencompnam),
		ivalue, index, ierror);
}

void
pxfeintset_(
	_f_int	*jhandle,
	char	*compnam,
	_f_int	*ivalue,
	_f_int	*index,
	_f_int	*ierror,
	int	lencompnam)
{
	_PXFEINTGET(jhandle, _cptofcd(compnam, lencompnam),
		ivalue, index, ierror);
}

void
pxfaintget_(
	_f_int	*jhandle,
	char	*compnam,
	_f_int	*iavalue,
	_f_int	*ialen,
	_f_int	*ierror,
	int	lencompnam)
{
	_PXFAINTGET(jhandle, _cptofcd(compnam, lencompnam),
		iavalue, ialen, ierror);
}

void
pxfaintset_(
	_f_int	*jhandle,
	char	*compnam,
	_f_int	*iavalue,
	_f_int	*ialen,
	_f_int	*ierror,
	int	lencompnam)
{
	_PXFAINTGET(jhandle, _cptofcd(compnam, lencompnam),
		iavalue, ialen, ierror);
}

#endif	/* end NOT _UNICOS */
