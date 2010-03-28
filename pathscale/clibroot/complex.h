/* USMID @(#)include/complex.h	100.0	07/11/97 00:26:19 */


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

#ifndef _COMPLEX_H
#define _COMPLEX_H

/* The complex data type is supported by scc 2.0 or later. */
#if __STDC__  && _RELEASE >= 2

#define complex   _Complex

#if _RELEASE >= 4
#define CMPLXF(x,y)	(((float)x)_Cmplx_((float)y))
#define CMPLX(x,y)	(((double)x)_Cmplx_((double)y))
#define CMPLXL(x,y)	(((long double)x)_Cmplx_((long double)y))
#else
#define CMPLXF(x,y)	((float)(x) + (float)(y) * 1.0fi) 
#define CMPLX(x,y)	((double)(x) + (double)(y) * 1.0i)
#define CMPLXL(x,y)	((long double)(x) + (long double)(y) * 1.0Li)
#endif

#include <sys/cdefs.h>

__BEGIN_DECLS
extern double cabs __((double complex _X));
extern double complex ccos __((double complex _X));
extern double complex cexp __((double complex _X));
extern double cimag __((double complex _X));
extern double complex clog __((double complex _X));
extern double complex conj __((double complex _X));
extern double complex cpow __((double complex _X, double complex _Y));
extern double creal __((double complex _X));
extern double complex csin __((double complex _X));
extern double complex csqrt __((double complex _X));
__END_DECLS

#endif /* __STDC__ && _RELEASE >= 2 */

#endif /* !_COMPLEX_H */
