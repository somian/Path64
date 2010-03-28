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


#pragma ident "@(#) libfi/char/f90_verify.c	92.2	07/30/99 10:09:59"

#include <fortran.h>

typedef struct
{
    char *ptr;   		/* character portion */
    unsigned long   len;	/* its length */
} fcd_t;
  
extern _f_int _VERIFY( fcd_t  str1,
		       fcd_t  str2,
		       _f_log *back );

_f_int _F90_VERIFY( char    *str1,
	            char    *str2,
	            _f_int  *back,
	            _f_int  len1,
	            _f_int  len2 )
{
    fcd_t  fcd1, fcd2;

    fcd1.ptr = str1;
    fcd1.len = len1;
    fcd2.ptr = str2;
    fcd2.len = len2;
    
    return( _VERIFY( fcd1, fcd2, back ) );
}


#ifdef	_F_INT4
extern _f_int4 _VERIFY_4( fcd_t  str1,
		          fcd_t  str2,
		          _f_log *back );

_f_int4 _F90_VERIFY_4( char    *str1,
	               char    *str2,
	               _f_int  *back,
	               _f_int  len1,
	               _f_int  len2 )
{
    fcd_t  fcd1, fcd2;

    fcd1.ptr = str1;
    fcd1.len = len1;
    fcd2.ptr = str2;
    fcd2.len = len2;
    
    return( _VERIFY_4( fcd1, fcd2, back ) );
}
#endif


#ifdef	_F_INT8
extern _f_int8 _VERIFY_8( fcd_t  str1,
		          fcd_t  str2,
		          _f_log *back );

_f_int8 _F90_VERIFY_8( char    *str1,
	               char    *str2,
	               _f_int  *back,
	               _f_int  len1,
	               _f_int  len2 )
{
    fcd_t  fcd1, fcd2;

    fcd1.ptr = str1;
    fcd1.len = len1;
    fcd2.ptr = str2;
    fcd2.len = len2;
    
    return( _VERIFY_8( fcd1, fcd2, back ) );
}
#endif

#ifdef	_F_INT2
extern _f_int2 _VERIFY_2( fcd_t  str1,
		          fcd_t  str2,
		          _f_log *back );

_f_int2 _F90_VERIFY_2( char    *str1,
	               char    *str2,
	               _f_int  *back,
	               _f_int  len1,
	               _f_int  len2 )
{
    fcd_t  fcd1, fcd2;

    fcd1.ptr = str1;
    fcd1.len = len1;
    fcd2.ptr = str2;
    fcd2.len = len2;
    
    return( _VERIFY_2( fcd1, fcd2, back ) );
}
#endif

#ifdef	_F_INT1
extern _f_int1 _VERIFY_1( fcd_t  str1,
		          fcd_t  str2,
		          _f_log *back );

_f_int1 _F90_VERIFY_1( char    *str1,
	               char    *str2,
	               _f_int  *back,
	               _f_int  len1,
	               _f_int  len2 )
{
    fcd_t  fcd1, fcd2;

    fcd1.ptr = str1;
    fcd1.len = len1;
    fcd2.ptr = str2;
    fcd2.len = len2;
    
    return( _VERIFY_1( fcd1, fcd2, back ) );
}
#endif

#if	defined(__mips) || defined(_LITTLE_ENDIAN)

extern _f_int4 _verify90(char *str1, _f_int4 len1, char *str2, _f_int4 len2, _f_int4 back);

_f_int4
_verify90(char *str1,
	_f_int4 len1,
	char *str2,
	_f_int4 len2,
	_f_int4 back)
{
	_f_int4 back1 = back;
	return (_F90_VERIFY(str1, str2, &back1, len1, len2));
}

#endif	/* __mips */
