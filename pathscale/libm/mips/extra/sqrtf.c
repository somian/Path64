
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

#include <math.h>

#ifdef mips
extern	float	fsqrt(float);
extern	float	sqrtf(float);

#pragma weak fsqrt = __sqrtf
#pragma weak sqrtf = __sqrtf
#endif

#if defined(BUILD_OS_DARWIN) /* Mach-O doesn't support aliases */
extern  float  __sqrtf(float);
#pragma weak sqrtf
float sqrtf( float x )
{
  return __sqrtf(x);
}
#elif defined(__GNUC__)
extern  float  __sqrtf(float);
float    sqrtf(float) __attribute__ ((weak, alias ("__sqrtf")));
#endif

float
__sqrtf( float x )
{
        return ( (float)sqrt( (double)x ) );
}
