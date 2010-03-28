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


#pragma ident "@(#) libfi/char/f90_ichar.c	92.1	07/08/99 10:41:51"

#include <fortran.h>

/*
 *      Note.  As of 4/1/94, the Sparc F90 compiler no longer generates a call
 *	to _F90_ICHAR to process the ICHAR intrinsic function.
 */
_f_int _F90_ICHAR_( char *str, int len )
{
    return((_f_int) *str);
}

#ifdef	_F_INT4
_f_int4 _F90_ICHAR_4_( char *str, _f_int4 len )
{
    return((_f_int4) *str);
}
#endif


#ifdef	_F_INT8
_f_int8 _F90_ICHAR_8_( char *str, _f_int8 len )
{
    return((_f_int8) *str);
}
#endif

