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


/* USMID @(#) libfi/include/intrin.h	92.0	10/08/98 14:37:14 */
#define TYPE_I	1
#define TYPE_J	2
#define TYPE_S	3
#define TYPE_D	4
#define TYPE_C	5
#define TYPE_Z	6
#define TYPE_L	7
#define TYPE_H	8
#define TYPE_U	9
#define TYPE_W	10

/*
 * Macro for memory allocation -- compatible with malloc()
 */

#define MALLOC	malloc

/*
 * Macro for error processing
 */

#include<liberrno.h>
/* 
 * The call to _ferror will be the real code, after mod is integrated to
 * define the error codes used by these routines
 */
#define ERROR(CODE) _lerror( _LELVL_ABORT, CODE)
