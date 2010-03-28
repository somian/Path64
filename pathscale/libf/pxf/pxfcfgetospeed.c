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


#pragma ident "@(#) libf/pxf/pxfcfgetospeed.c	92.2	06/29/99 11:36:06"

/*
 *  PXFCFGETISPEED  -- get input baud rate
 *  PXFCFSETISPEED  -- set input baud rate
 *  PXFCFGETOSPEED  -- get output baud rate
 *  PXFCFSETOSPEED  -- set output baud rate
 *             (section 7.1.3 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *
 *     SUBROUTINE PXFCFGETISPEED(jtermios, iospeed, ierror)
 *     INTEGER jtermios, iospeed, ierror
 *
 *     SUBROUTINE PXFCFSETISPEED(jtermios, ispeed, ierror)
 *     INTEGER jtermios, ispeed, ierror
 *
 *     SUBROUTINE PXFCFGETOSPEED(jtermios, iospeed, ierror)
 *     INTEGER jtermios, iospeed, ierror
 *
 *     SUBROUTINE PXFCFSETOSPEED(jtermios, ispeed, ierror)
 *     INTEGER jtermios, ispeed, ierror
 *
 *  Description:
 *
 *  The PXFCF...SPEED routines use the c functions to get or set the
 *  baud rates in the termios structure.  Symbolic names for the baud
 *  rates can be obtained through calls to PXFCONST.
 *
 *  The arguments are:
 *
 *      jtermios -  default integer input variable containing a handle
 *                  created by PXFSTRUCTCREATE('termios',...).
 *      ispeed   -  default input integer variable for a baud rate.
 *      iospeed  -  default output integer variable specifying a baud
 *                  rate.
 *      ierror   -  default integer output variable that contains zero
 *                  if the operation was successful or nonzero if the
 *                  operation was not successful.
 *
 *   PXFCF...SPEED routines may return one of the following error values:
 *   No errors are returned for bad baud rates.  The PXFCFSETISPEED and
 *   PXFCFSETOSPEED return 0 if successful; otherwise, they return -1.
 *
 *   EBADHANDLE The jtermios argument is invalid.
 *
 *   EINVAL     Problems occurred with the baud rate.
 *
 */

#include <fortran.h>
#include <liberrno.h>
#include <string.h>
#include <sys/termios.h>
#include <termios.h>
#include "pxfstruct.h"
#include "table.h"

#ifdef _UNICOS
void
PXFCFGETISPEED(
#else	/* _UNICOS */
void
pxfcfgetispeed_(
#endif	/* _UNICOS */
	_f_int	*jtermios,
	_f_int	*iospeed,
	_f_int	*ierror)
{
	speed_t	stat;
	struct  pxfhandle pxfhand;
	struct termios *trmios;
	*ierror	= 0;
	*iospeed	= 0;
	pxfhand	= _pxfhandle_table_lookup(&_pxfhandle_table, *jtermios);
	if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_TERMIOS) {
		*ierror	= EBADHANDLE;
		return;
	}

	trmios	= pxfhand.pxfstructptr;
	if (stat = cfgetispeed(trmios) == -1)
		*ierror	= EINVAL;
	else
		*iospeed	= stat;
	return;
}

#ifdef _UNICOS
void
PXFCFGETOSPEED(
#else	/* _UNICOS */
void
pxfcfgetospeed_(
#endif	/* _UNICOS */
	_f_int	*jtermios,
	_f_int	*iospeed,
	_f_int	*ierror)
{
	speed_t	stat;
	struct  pxfhandle pxfhand;
	struct termios *trmios;
	*ierror	= 0;
	*iospeed	= 0;
	pxfhand	= _pxfhandle_table_lookup(&_pxfhandle_table, *jtermios);
	if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_TERMIOS) {
		*ierror	= EBADHANDLE;
		return;
	}

	trmios	= pxfhand.pxfstructptr;
	if (stat = cfgetospeed(trmios) == -1)
		*ierror	= EINVAL;
	else
		*iospeed	= stat;
	return;
}

#ifdef _UNICOS
void
PXFCFSETISPEED(
#else	/* _UNICOS */
void
pxfcfsetispeed_(
#endif	/* _UNICOS */
	_f_int	*jtermios,
	_f_int	*ispeed,
	_f_int	*ierror)
{
	int	stat;
	speed_t	newspeed;
	struct  pxfhandle pxfhand;
	struct termios *trmios;
	*ierror	= 0;
	newspeed	= *ispeed;
	pxfhand	= _pxfhandle_table_lookup(&_pxfhandle_table, *jtermios);
	if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_TERMIOS) {
		*ierror	= EBADHANDLE;
		return;
	}

	trmios	= pxfhand.pxfstructptr;
	if (stat = cfsetispeed(trmios,newspeed) == -1)
		*ierror	= EINVAL;
	return;
}

#ifdef _UNICOS
void
PXFCFSETOSPEED(
#else	/* _UNICOS */
void
pxfcfsetospeed_(
#endif	/* _UNICOS */
	_f_int	*jtermios,
	_f_int	*ispeed,
	_f_int	*ierror)
{
	int	stat;
	speed_t	newspeed;
	struct  pxfhandle pxfhand;
	struct termios *trmios;
	*ierror	= 0;
	newspeed	= *ispeed;
	pxfhand	= _pxfhandle_table_lookup(&_pxfhandle_table, *jtermios);
	if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_TERMIOS) {
		*ierror	= EBADHANDLE;
		return;
	}

	trmios	= pxfhand.pxfstructptr;
	if (stat = cfsetospeed(trmios,newspeed) == -1)
		*ierror	= EINVAL;
	return;
}

