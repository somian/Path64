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


/* ====================================================================
 * ====================================================================
 *
 * Module: pow_qi.c
 *
 * Description:  source code for pow_qi and pow_ql functions
 *
 * ====================================================================
 * ====================================================================
 */

#include "quad.h"

/* ====================================================================
 *
 * FunctionName    pow_qi
 *
 * Description    computes x**n for long double x and int n
 *
 * ====================================================================
 */

/* by value only */

long double __pow_qi(double ahi, double alo, int n)
{
  ldquad  x;
  long double  result;

  x.q.hi = ahi;
  x.q.lo = alo;
  result = __qpow(x.ld, (long double)n);
  return (result);
}

/* ====================================================================
 *
 * FunctionName    pow_ql
 *
 * Description    computes x**n for long double x and long long n
 *
 * ====================================================================
 */
/* by value only */

long double __pow_ql(double ahi, double alo, long long n)
{
  ldquad  x;
  long double  result;

  x.q.hi = ahi;
  x.q.lo = alo;
  result = __qpow(x.ld, (long double)n);
  return (result);
}
