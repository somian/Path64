/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#pragma ident "@(#) libfi/char/f90_index.c	92.2	07/08/99 10:41:51"

#include <fortran.h>
#include <stddef.h>

extern int _INDEX( _fcd fstr1, _fcd fstr2, _f_log *fback );

/*
 *	_F90_INDEX	- for f90 intrinsic when explicitly called on Sparc
 *
 *	The F90 compiler generates a call to _F90_INDEX to process explicit
 *	calls to the INDEX intrinsic, which have one optional argument.	
 *
 *	Note that the compiler passes a BACK specifier in fpack--not a pointer
 *	to it.
 */
_f_int
_F90_INDEX(
char   *str1,
char   *str2,
_f_log *fback,
int    len1,
int    len2 )
{
    return( _INDEX( _cptofcd(str1, len1), _cptofcd(str2, len2), fback ) );
}

/*
 *	_F90_INDEX_	- for f90 intrinsic when passed as an argument
 *
 *	The implicit call to the INDEX intrinsic in these cases (through
 *	_F90_INDEX_) must be made with only two arguments.  This minimal
 *	support for passing INDEX as an actual argument is in Fortran 90
 *	for upward compatibility with the Fortran 77 standard.
 */
_f_int
_F90_INDEX_(
char   *str1,
char   *str2,
int    len1,
int    len2 )
{
    return( _INDEX( _cptofcd(str1, len1), _cptofcd(str2, len2), NULL ) );
}

#ifdef	_F_INT4
extern int _INDEX_4( _fcd fstr1, _fcd fstr2, _f_log *fback );

/*
 *	_F90_INDEX_4	- for f90 intrinsic when explicitly called on Sparc
 *
 *	The F90 compiler generates a call to _F90_INDEX_4 to process explicit
 *	calls to the INDEX_4 intrinsic, which have one optional argument.	
 *
 *	Note that the compiler passes a BACK specifier in fpack--not a pointer
 *	to it.
 */
_f_int4
_F90_INDEX_4(
char   *str1,
char   *str2,
_f_log *fback,
int    len1,
int    len2 )
{
    return( _INDEX_4( _cptofcd(str1, len1), _cptofcd(str2, len2), fback ) );
}

/*
 *	_F90_INDEX_4_	- for f90 intrinsic when passed as an argument
 *
 *	The implicit call to the INDEX intrinsic in these cases (through
 *	_F90_INDEX_4_) must be made with only two arguments.  This minimal
 *	support for passing INDEX as an actual argument is in Fortran 90
 *	for upward compatibility with the Fortran 77 standard.
 */
_f_int4
_F90_INDEX_4_(
char   *str1,
char   *str2,
int    len1,
int    len2 )
{
    return( _INDEX_4( _cptofcd(str1, len1), _cptofcd(str2, len2), NULL ) );
}
#endif


#ifdef	_F_INT8
extern int _INDEX_8( _fcd fstr1, _fcd fstr2, _f_log *fback );

/*
 *	_F90_INDEX_8	- for f90 intrinsic when explicitly called on Sparc
 *
 *	The F90 compiler generates a call to _F90_INDEX_8 to process explicit
 *	calls to the INDEX_8 intrinsic, which have one optional argument.	
 *
 *	Note that the compiler passes a BACK specifier in fpack--not a pointer
 *	to it.
 */
_f_int4
_F90_INDEX_8(
char   *str1,
char   *str2,
_f_log *fback,
int    len1,
int    len2 )
{
    return( _INDEX_8( _cptofcd(str1, len1), _cptofcd(str2, len2), fback ) );
}

/*
 *	_F90_INDEX_8_	- for f90 intrinsic when passed as an argument
 *
 *	The implicit call to the INDEX intrinsic in these cases (through
 *	_F90_INDEX_8_) must be made with only two arguments.  This minimal
 *	support for passing INDEX as an actual argument is in Fortran 90
 *	for upward compatibility with the Fortran 77 standard.
 */
_f_int8
_F90_INDEX_8_(
char   *str1,
char   *str2,
int    len1,
int    len2 )
{
    return( _INDEX_8( _cptofcd(str1, len1), _cptofcd(str2, len2), NULL ) );
}
#endif

#ifdef	_F_INT2
extern int _INDEX_2( _fcd fstr1, _fcd fstr2, _f_log *fback );

/*
 *	_F90_INDEX_2	- for f90 intrinsic when explicitly called on Sparc
 *
 *	The F90 compiler generates a call to _F90_INDEX_2 to process explicit
 *	calls to the INDEX_2 intrinsic, which have one optional argument.	
 *
 *	Note that the compiler passes a BACK specifier in fpack--not a pointer
 *	to it.
 */
_f_int2
_F90_INDEX_2(
char   *str1,
char   *str2,
_f_log *fback,
int    len1,
int    len2 )
{
    return( _INDEX_2( _cptofcd(str1, len1), _cptofcd(str2, len2), fback ) );
}

/*
 *	_F90_INDEX_2_	- for f90 intrinsic when passed as an argument
 *
 *	The implicit call to the INDEX intrinsic in these cases (through
 *	_F90_INDEX_2_) must be made with only two arguments.  This minimal
 *	support for passing INDEX as an actual argument is in Fortran 90
 *	for upward compatibility with the Fortran 77 standard.
 */
_f_int2
_F90_INDEX_2_(
char   *str1,
char   *str2,
int    len1,
int    len2 )
{
    return( _INDEX_2( _cptofcd(str1, len1), _cptofcd(str2, len2), NULL ) );
}
#endif

#ifdef	_F_INT1
extern int _INDEX_1( _fcd fstr1, _fcd fstr2, _f_log *fback );

/*
 *	_F90_INDEX_1	- for f90 intrinsic when explicitly called on Sparc
 *
 *	The F90 compiler generates a call to _F90_INDEX_1 to process explicit
 *	calls to the INDEX_1 intrinsic, which have one optional argument.	
 *
 *	Note that the compiler passes a BACK specifier in fpack--not a pointer
 *	to it.
 */
_f_int1
_F90_INDEX_1(
char   *str1,
char   *str2,
_f_log *fback,
int    len1,
int    len2 )
{
    return( _INDEX_1( _cptofcd(str1, len1), _cptofcd(str2, len2), fback ) );
}

/*
 *	_F90_INDEX_1_	- for f90 intrinsic when passed as an argument
 *
 *	The implicit call to the INDEX intrinsic in these cases (through
 *	_F90_INDEX_1_) must be made with only two arguments.  This minimal
 *	support for passing INDEX as an actual argument is in Fortran 90
 *	for upward compatibility with the Fortran 77 standard.
 */
_f_int1
_F90_INDEX_1_(
char   *str1,
char   *str2,
int    len1,
int    len2 )
{
    return( _INDEX_1( _cptofcd(str1, len1), _cptofcd(str2, len2), NULL ) );
}
#endif

#if defined(__mips) || defined(_LITTLE_ENDIAN)

extern _f_int4 _index90(char *str1, _f_int4 len1, char *str2, _f_int4 len2, _f_int4 back);

_f_int4
_index90(char *str1,
	_f_int4 len1,
	char *str2,
	_f_int4 len2,
	_f_int4 back)
{
#ifndef KEY
  _f_int4 back1 = back;
  return (_F90_INDEX(str1, str2, &back1, len1, len2));
#else
  const int len = len1 - len2;
  int j = 0;

  if( len < 0 || len2 <= 0 ){
    return 0;
  }

  if(back == 0) {
    for( j = 0; j <= len; j++ ){
      int i, p = j;
      for( i = 0; i < len2; i++,p++ ){
        if( str1[p] != str2[i] )
	  break;
      }

      if( i == len2 )
        return j+1;
    }
  } else {
    int l2 = len2-1;
    for( j = len1-1; j >= l2; j-- ){
      int i, p = j;
      for( i = len2-1; i >= 0; i--,p-- ){
        if( str1[p] != str2[i] )
	  break;
      }

      if( i == -1 )
        return p+2;
    }
  }

  return 0;
#endif
}

#endif	/* __mips or _LITTLE_ENDIAN */
