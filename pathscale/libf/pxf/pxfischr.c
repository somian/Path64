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


#pragma ident "@(#) libf/pxf/pxfischr.c	92.1	06/29/99 11:36:06"

#include <liberrno.h>
#include <errno.h>
#include <fortran.h>
#include <sys/stat.h>

/*
 *  PXFISCHR  -- Test for character special file
 *  (section 5.6.1 of Posix 1003.9-1992)
 *
 *  Synopsis:
 *     LOGICAL FUNCTION PXFISCHR(M)
 *     INTEGER M
 *
 *  Function description:
 *  The logical function PXFISCHR checks if a file is a
 *  character special file. The value M should be supplied by
 *  the st_mode component of the stat structure used by
 *  the PXFSTAT routine. If the file is a character special
 *  file, the PXFISBLK returns a logical true, otherwise
 *  a logical false is returned.
 *
 *  Description of arguments:
 *  M is an integer input variable containing the file mode.
 *
 *  Return values:
 *  If the file is a character special file, the PXFISCHR returns
 *  a logical true, otherwise a logical false is returned.
 */

#ifdef _UNICOS
_f_int
PXFISCHR(
#else
_f_int
_PXFISCHR(
#endif
	  _f_int *M
)
{
  return _btol(S_ISCHR((int)*M));
}

#ifndef _UNICOS
_f_int
pxfischr_(
	  _f_int *M
)
{
  return _PXFISCHR(M);
}
#endif
