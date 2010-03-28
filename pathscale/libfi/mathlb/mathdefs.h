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


/* USMID @(#) libfi/mathlb/mathdefs.h	92.0	10/08/98 14:37:14 */
#include <fortran.h>

typedef struct
{
  _f_real4 real;
  _f_real4 imag;
} complex_t;

typedef struct
{
  _f_real8 real;
  _f_real8 imag;
} dbl_complex_t;

typedef struct
{
  _f_real4 real;
  _f_real4 imag;
} h_complex_t;

typedef struct
{
  _f_real8 real;
  _f_real8 imag;
} c_complex_t;

#ifdef _F_COMP16
typedef struct
{
  _f_real16 real;
  _f_real16 imag;
} d_complex_t;
#endif
