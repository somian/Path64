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


#pragma ident "@(#) libfi/char/f90_len_trim.c	92.1	07/08/99 10:41:51"

#include <fortran.h>

extern _f_int _LEN_TRIM_( _fcd f );

int
_F90_LEN_TRIM_( char *ptr, int len )
{
    return( _LEN_TRIM_( _cptofcd(ptr, len) ) );
}


#ifdef	_F_INT4
extern _f_int4 _LEN_TRIM_4_( _fcd f );

_f_int4
_F90_LEN_TRIM_4_( char *ptr, int len )
{
    return( _LEN_TRIM_4_( _cptofcd(ptr, len) ) );
}
#endif


#ifdef	_F_INT8
extern _f_int _LEN_TRIM_8_( _fcd f );

_f_int8
_F90_LEN_TRIM_8_( char *ptr, int len )
{
    return( _LEN_TRIM_8_( _cptofcd(ptr, len) ) );
}
#endif
