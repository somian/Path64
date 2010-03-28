/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


/* $Header: /home/bos/bk/kpro64-pending/include/cmplrs/host.h 1.6 04/12/21 14:57:36-08:00 bos@eng-25.internal.keyresearch.com $ */

#ifndef _HOST_H
#define _HOST_H
/*
**  host.h
**
**  Basic type declarations, macros, ... to promote reuse and
**  portability throughout the compiler. 
**
**  Include this file before all others.
*/

#if (defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS))

typedef int boolean;
#ifndef TRUE
#define TRUE	(1)
#define FALSE	(0)
#endif

typedef char *string;
typedef char char_t;
#ifndef __sgi
/* gb - sys/types.h defines these types, which is included by <signal.h> */
typedef unsigned char uchar_t;
#endif 
typedef signed short short_t;
#ifndef __sgi
typedef unsigned short ushort_t;
#endif
typedef signed int int_t;
#ifndef __sgi
typedef unsigned int uint_t;
#endif
typedef signed long long_t;
#ifndef __sgi
typedef unsigned long ulong_t;
#else
#include <sys/types.h>
#endif

#if defined(_LONGLONG)
typedef signed long long longlong_t;
typedef unsigned long long ulonglong_t;
#else
typedef signed long longlong_t;
typedef unsigned long ulonglong_t;
#endif

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef longlong_t int64;
typedef ulonglong_t uint64;

typedef void *pointer;          /* a generic pointer type */

typedef int32 fsize_t; /* Size of a "hidden length" when passing Fortran CHARACTER arguments */
/* Another reasonable choice:  (requires <string.h>)
**    typedef size_t fsize_t;
*/


#endif

#if defined(_LANGUAGE_PASCAL)

#if defined(_LONGLONG)
type long_integer = integer64;
type long_cardinal = cardinal64;
#else
type long_integer = integer;
type long_cardinal = cardinal;
#endif
#endif

#endif

