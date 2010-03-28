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


//* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#include <alloca.h>
#include "ipa_section.h"
#include "ipa_lno_file.h"
#include "ipa_lno_util.h"
#include "be_util.h"
#include "opt_du.h"

inline mINT32 myabs(mINT32 i)
{
  return i < 0 ? -i : i;
}

inline mINT64 myabs(mINT64 i)
{
  return i < 0 ? -i : i;
}

INT32 Gcd(mINT32 i, mINT32 j)
{
  i = myabs(i);
  j = myabs(j);

  if (i == 0)
    return j;
  if (j == 0)
    return i;

  if (i > j) {
    INT32 t = i;
    i = j;
    j = t;
  }

  do {
    INT32 t = i;
    i = j%i;
    j = t;
  } while (i);

  return j;
}

INT64 Gcd(mINT64 i, mINT64 j)
{
  i = myabs(i);
  j = myabs(j);

  if (i == 0)
    return j;
  if (j == 0)
    return i;

  if (i > j) {
    INT64 t = i;
    i = j;
    j = t;
  }

  do {
    INT64 t = i;
    i = j%i;
    j = t;
  } while (i);

  return j;
} 

INT32 Gcd(const mINT32* i, INT cnt)
{
  FmtAssert(cnt >= 1, ("vector Gcd() with no args"));

  INT32 g = Gcd(mINT32(0), i[0]);
  for (INT32 j = 1; j < cnt; j++)
    g = Gcd(g, i[j]);
  return g;
}

INT64 Gcd(const mINT64* i, INT cnt)
{
  FmtAssert(cnt >= 1, ("vector Gcd() with no args"));

  INT64 g = Gcd(mINT64(0), i[0]);
  for (INT32 j = 1; j < cnt; j++)
    g = Gcd(g, i[j]);
  return g;
}

INT32 Lcm(mINT32 i, mINT32 j)
{
  INT32 g = Gcd(i, j);
  return g == 0 ? 0 : myabs(i/g * j);
}

INT64 Lcm(mINT64 i, mINT64 j)
{
  INT64 g = Gcd(i, j);
  return g == 0 ? 0 : myabs(i/g * j);
}

INT32 Lcm(const mINT32* i, INT cnt)
{
  FmtAssert(cnt >= 1, ("vector Lcm() with no args"));

  INT32 l = i[0];
  for (INT32 j = 1; j < cnt; j++) {
    INT32 g = Gcd(l, i[j]);
    INT32 tmp = l/g * i[j];
    FmtAssert(tmp/i[j] == l/g, ("overflow in Lcm()"));
    l = tmp;
  }
  return myabs(l);
}

INT64 Lcm(const mINT64* i, INT cnt)
{
  FmtAssert(cnt >= 1, ("vector Lcm() with no args"));

  INT64 l = i[0];
  for (INT32 j = 1; j < cnt; j++) {
    INT64 g = Gcd(l, i[j]);
    INT64 tmp = l/g * i[j];
    FmtAssert(tmp/i[j] == l/g, ("overflow in Lcm()"));
    l = tmp;
  }
  return myabs(l);
}

