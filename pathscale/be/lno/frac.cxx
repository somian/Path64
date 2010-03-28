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


// -*-C++-*-

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include <sys/types.h>
#include "frac.h"
#include "lnoutils.h"
#include "ipa_lno_util.h"

BOOL FRAC::Exception = FALSE;

// ********************* Fraction reduction

void FRAC::_reduce2()
{
  if (_d == 0) {
    if (Exception == FALSE) {
      DevWarn("_reduce2() passed zero denominator");
      Exception = TRUE;
    }
    _d = 1;
  }

  // neither is zero
  BOOL neg = _n < 0 ? _d > 0 : _d < 0;

  FRAC_ETY i1 = _n < 0 ? -_n : _n;
  FRAC_ETY i2 = _d < 0 ? -_d : _d;
  FRAC_ETY g = Gcd(i1, i2);

  *this = _checksz((neg ? -i1/g : i1/g), (i2/g));
}

void FRAC::_reduce2(FRAC_ITY n, FRAC_ITY d)
{
  if (_d == 0) {
    if (Exception == FALSE) {
      DevWarn("_reduce2() passed zero denominator");
      Exception = TRUE;
    }
    _d = 1;
  }

  // neither is zero
  INT8 neg = n < 0 ? d > 0 : d < 0;

  FRAC_ITY i1 = n < 0 ? -n : n;
  FRAC_ITY i2 = d < 0 ? -d : d;
  FRAC_ITY g = Gcd(i1, i2);

  *this = _checksz((neg ? -i1/g : i1/g), (i2/g));
}

// ********************* Fraction operations

FRAC FRAC::operator +(FRAC f) const
{
  FRAC_ITY n = (FRAC_ITY) _n * f._d + (FRAC_ITY) f._n * _d;
  FRAC_ITY d = (FRAC_ITY) _d * f._d;
  return FRAC(_checksz(n), _checksz(d));
}

FRAC FRAC::operator -(FRAC f) const
{
  FRAC_ITY n = (FRAC_ITY) _n * f._d - (FRAC_ITY) f._n * _d;
  FRAC_ITY d = (FRAC_ITY) _d * f._d;
  return FRAC(_checksz(n), _checksz(d));
}

FRAC FRAC::operator *(FRAC f) const
{
  FRAC_ITY n = (FRAC_ITY) _n * f._n;
  FRAC_ITY d = (FRAC_ITY) _d * f._d;
  return FRAC(_checksz(n), _checksz(d));
}

FRAC FRAC::operator /(FRAC f) const
{
  // the constructor catches division by zero
  FRAC_ITY n = (FRAC_ITY) _n * f._d;
  FRAC_ITY d = (FRAC_ITY) _d * f._n;
  return FRAC(_checksz(n), _checksz(d));
}

// ********************* Fraction printing

void FRAC::Print(FILE* f) const
{
  if (_d == 1)
    fprintf(f, " %d ", _n);
  else
    fprintf(f, "%d/%d", _n, _d);
}

char* FRAC::Print(char* s) const
{
  if (_d == 1)
    sprintf(s, " %d ", _n);
  else
    sprintf(s, "%d/%d", _n, _d);

  return s + ::strlen(s);
}

