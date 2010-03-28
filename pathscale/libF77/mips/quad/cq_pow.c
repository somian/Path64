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
 * Module: cq_pow.c
 *
 * Description:	source code for complex quad power function
 *
 * ====================================================================
 * ====================================================================
 */

#include "quad.h"

  /* complex *32 power function */

/* ====================================================================
 *
 * FunctionName		cq_abs
 *
 * Description		computes quad complex power function of args
 *
 * ====================================================================
 */

extern qcomplex __qcis(long double arg);

qcomplex __cqpow(long double aqreal, long double aqimag, long double bqreal, long double bqimag)
{
  long double logr, logi, x, y;
  qcomplex r, qc;

  logr = __qlog( __qhypot(aqreal, aqimag));
  logi = __qatan2( aqimag, aqreal);

  x = __qexp( logr*bqreal - logi*bqimag);
  y = logr*bqimag + logi*bqreal;

  qc = __qcis(y);
  r.qreal = x*qc.qreal;
  r.qimag = x*qc.qimag;
  return r;
}

void __cq_pow(qcomplex *r, qcomplex *a, qcomplex *b)
{
  *r = __cqpow (a->qreal, a->qimag, b->qreal, b->qimag);
}
