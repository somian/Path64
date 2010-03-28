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
 * Module: cq_sqrt.c
 *
 * Description:  source code for complex quad sqrt function
 *
 * ====================================================================
 * ====================================================================
 */

#include "quad.h"

/* intrinsic CQSQRT */

qcomplex __cqsqrt(long double zqreal, long double zqimag)
{
  long double mag;
  qcomplex result;

  if ((mag = __qhypot(zqreal, zqimag)) == 0.0L) {
    result.qreal = result.qimag = 0.0L;
  } else if (zqreal > 0.0L) {
    result.qreal = __qsqrt(0.5L*(mag + zqreal));
    result.qimag = (zqimag/result.qreal)*0.5L;
  } else {
    result.qimag = __qsqrt(0.5L*(mag - zqreal));
    if (zqimag < 0.0L)
      result.qimag = - result.qimag;
    result.qreal = (zqimag/result.qimag)*0.5L;
  }
  return result;
}

void __cq_sqrt(qcomplex *result, qcomplex *z)
{
  *result = __cqsqrt(z->qreal, z->qimag);
}
