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
 * Module: cq_div.c
 *
 * Description:  source code for complex quad division
 *
 * ====================================================================
 * ====================================================================
 */

#include <sys/fpu.h>
#include "quad.h"

  /* complex *32 division */

/* ====================================================================
 *
 * FunctionName    cq_div
 *
 * Description    computes complex quotient of args
 *
 * ====================================================================
 */

qcomplex __cqdiv(long double xqreal, long double xqimag, long double yqreal, long double yqimag)
{
  long double tmp;
  unsigned int m, n;
  qcomplex result;

  if ((xqreal != xqreal) || (xqimag != xqimag) ||
       (yqreal != yqreal) || (yqimag != yqimag)) {
    result.qreal = __libm_qnan_ld;
    result.qimag = __libm_qnan_ld;
    return result;
  }

  if ((yqreal == 0.0L) && (yqimag == 0.0L)) {
    result.qreal = xqreal/__libm_zero_ld;
    result.qimag = xqimag/__libm_zero_ld;
    return result;
  }

  if (yqreal == 0.0L) {
    result.qreal = xqimag/yqimag;
    result.qimag = -(xqreal/yqimag);
    return result;
  }

  if (yqimag == 0.0L) {
    result.qreal = xqreal/yqreal;
    result.qimag = xqimag/yqreal;
    return result;
  }

  if (__qabs(yqreal) <= __qabs(yqimag)) {
    /* turn off traps on underflow while computing
       yqreal/yqimag
    */

    m = get_fpc_csr();
    n = (m & 0xfffffeff);
    (void)set_fpc_csr(n);
    tmp = yqreal/yqimag;
    (void)set_fpc_csr(m);
    result.qreal = (xqimag + xqreal*tmp)/(yqimag + yqreal*tmp);
    result.qimag = (-xqreal + xqimag*tmp)/(yqimag + yqreal*tmp);
    return result;
  }

  /* turn off traps on underflow while computing
     yqimag/yqreal
  */

  m = get_fpc_csr();
  n = (m & 0xfffffeff);
  (void)set_fpc_csr(n);

  tmp = yqimag/yqreal;
  (void)set_fpc_csr(m);

  result.qreal = (xqreal + xqimag*tmp)/(yqreal + yqimag*tmp);
  result.qimag = (xqimag - xqreal*tmp)/(yqreal + yqimag*tmp);

  return result;
}

void __cq_div(qcomplex *result, qcomplex *x, qcomplex *y)
{
  *result = __cqdiv(x->qreal, x->qimag, y->qreal, y->qimag);
}
