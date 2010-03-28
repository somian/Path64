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


#pragma ident "@(#) libu/util/asnquery.c	92.1	07/07/99 13:18:33"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fortran.h>
#include <liberrno.h>
#include <cray/assign.h>

#define NOT_FOUND	-1
#define FOUND		0

/*
 *  ASNQUNIT	- returns the assign attributes for a Fortran unit number.
 *
 *  Call from Fortran:
 *
 *	CALL ASNQUNIT(IUN, ATTR, ISTAT)
 *
 *  Parameters
 *
 *	IUN		(I) unit number
 *	ATTR		(O) receives the assign options for this unit number
 *	ISTAT		(O) 0 if any options were found, -1 if not found,
 *			    >0 error code on error.
 */
void
#ifdef	_UNICOS
ASNQUNIT(
	_f_int	*iun,
	_fcd	attrs,
	_f_int	*istat)
{
#else	/* _SOLARIS */
asnqunit_(
	_f_int	*iun,
	char	*attrptr,
	_f_int	*istat,
	int	attrlen)
{
	_fcd attrs	= _cptofcd(attrptr, attrlen);
#endif	/* _SOLARIS */

	int	ret;
	unum_t	unum;
	char	*atstr;

	unum	= *iun;

	ret	= _get_a_options(0, NULL, unum, 0, NULL, &atstr, _LELVL_RETURN);

	switch (ret) {
	case -1:	/* an error condition was encountered */
		*istat	= errno;
		break;
	case 0:		/* attributes were not found */
		*istat	= NOT_FOUND;
		break;
	case 1:		/* attributes were found */
		*istat	= FOUND;
		if (_c2fcpy(atstr, attrs) == -1)
			*istat	= ERAS_ATTSPC;
		free(atstr);
		break;
	}

	if (*istat != FOUND)
		(void)_c2fcpy("", attrs);	/* fill with blanks */

	return;
}

/*
 *  ASNQFILE	- returns the assign attributes for a file name.
 *
 *  Call from Fortran:
 *
 *	CALL ASNQFILE(FNAME, ATTR, ISTAT)
 *
 *  Parameters
 *
 *	FNAME		(I) file name
 *	ATTR		(O) receives the assign options for this unit number
 *	ISTAT		(O) 0 if any options were found, -1 if not found,
 *			    >0 error code on error.
 */
void
#ifdef	_UNICOS
ASNQFILE(
	_fcd	fname,
	_fcd	attrs,
	_f_int	*istat)
	
{
#else	/* _SOLARIS, __mips, etc. */
asnqfile_(
	char	*fnamptr,
	char	*attrptr,
	_f_int	*istat,
	int	fnamlen,
	int	attrlen)
{
	_fcd attrs	= _cptofcd(attrptr, attrlen);
	_fcd fname	= _cptofcd(fnamptr, fnamlen);
#endif	/* _SOLARIS */

	int	ret;
	char	*atstr;
	char	*cfname;

	if ((cfname = _fc_acopy(fname)) == NULL) {
		*istat	= FENOMEMY;
	}

	ret	= _get_a_options(0, cfname, (unum_t) 0, 0, NULL, &atstr,
			_LELVL_RETURN);

	free(cfname);

	switch (ret) {
	case -1:	/* an error condition was encountered */
		*istat	= errno;
		break;
	case 0:		/* attributes were not found */
		*istat	= NOT_FOUND;
		break;
	case 1:		/* attributes were found */
		*istat	= FOUND;
		if (_c2fcpy(atstr, attrs) == -1)
			*istat	= ERAS_ATTSPC;
		free(atstr);
		break;
	}

	if (*istat != FOUND)
		(void)_c2fcpy("", attrs);	/* fill with blanks */

	return;
}
