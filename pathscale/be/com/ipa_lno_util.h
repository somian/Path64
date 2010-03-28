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
#ifndef ipa_lno_util_INCLUDED
#define ipa_lno_util_INCLUDED

/** Exported types and functions:
***
***     INT32 Gcd(mINT32, mINT32)
***     INT64 Gcd(mINT64, mINT64)
***     INT32 Lcm(mINT32, mINT32)
***     INT64 Lcm(mINT64, mINT64)
***
***         Find the gcd and lcm of pairs.  May be positive, negative or zero.
***         gcd(0,x) = x.  lcm(0,x) = 0.
***
***     INT32 Gcd(const mINT32* vector, INT count)
***     INT64 Gcd(const mINT64* vector, INT count)
***     INT32 Lcm(const mINT32* vector, INT count)
***     INT64 Lcm(const mINT64* vector, INT count)
***
***         Find the gcd and lcm of vectors of integers.  Count must be
***         at least one.  The vectors may contain 0, positive or negatives.
**/

extern INT32 Gcd(mINT32, mINT32);
extern INT64 Gcd(mINT64, mINT64);
extern INT32 Gcd(const mINT32*, INT);
extern INT64 Gcd(const mINT64*, INT);

extern INT32 Lcm(mINT32, mINT32);
extern INT64 Lcm(mINT64, mINT64);
extern INT32 Lcm(const mINT32*, INT);
extern INT64 Lcm(const mINT64*, INT);

#endif
