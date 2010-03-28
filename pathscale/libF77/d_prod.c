/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* $Header: /home/bos/bk/kpro64-pending/libF77/d_prod.c 1.5 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

double d_prod(float *x, float *y)
{
  return((double)(*x)*(double)(*y));
}

#ifdef KEY /* Bug 2623 */
/*
 * The standard Fortran "dprod" intrinsic takes real*4 args and returns
 * a real*8 result. The __q_prod function in file mips/quad/q_prod.c
 * implements an extension which takes real*8 args and returns a real*16
 * result. Since we don't allow real*16, but we do provide an option -r8 which
 * promotes real*4 to real*8 but leaves real*8 alone, it seems user-unfriendly
 * not to provide a version of "dprod" which works with -r8. Here it is. If
 * we ever support real*16 and add the mips/quad files to the shared library,
 * the linker will give a "duplicate symbol error" to remind us to remove this
 * version.
 *
 * (The crayfe/fe90 sources use a cpp symbol _TARGET_OS_LINUX to enable code
 * which knows that real*16 is disallowed, but we can't use that symbol here
 * because it is defined within the front end itself.)
 */
double __q_prod(double *x, double *y)
{
  return (*x)*(*y);
}
#endif /* KEY Bug 2623 */
