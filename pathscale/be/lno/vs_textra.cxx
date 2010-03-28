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

/**
*** Vector space member functions and friends for which template functions are
*** not adequate.  Thus 'textra' => 'template extra'.
**/

/** $Revision: 1.6 $
*** $Date: 04/12/21 14:57:16-08:00 $
*** $Author: bos@eng-25.internal.keyresearch.com $
*** $Source: /home/bos/bk/kpro64-pending/be/lno/SCCS/s.vs_textra.cxx $
**/

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#define vs_textra_CXX      "vs_textra.cxx"
static const char *rcs_id =   vs_textra_CXX "$Revision: 1.6 $";


#include <sys/types.h>
#include "lnopt_main.h"
#include "vs.h"
#include "lnoutils.h"
#include "frac.h"
#include "ipa_lno_util.h"

const char* _Skip_Whitespace(const char* cp)
{
  INT c;

  while ((c = *cp) != '\0' && c != ' ' && c != '\n' && c != '\t')
    cp++;

  return cp;
}

template<>
void VECTOR_SPACE<double>::Print_Element(FILE* f, double e)
{
  fprintf(f, "%g", e);
}

template<>
void VECTOR_SPACE<FRAC>::Print_Element(FILE* f, FRAC e)
{
  e.Print(f);
}

template<>
void VECTOR_SPACE<double>::Reduce_Row(double*, INT)
{
  return;
}

template<>
void VECTOR_SPACE<FRAC>::Reduce_Row(FRAC* row, INT elts)
{
  INT j;

  // make all entries integral
  FRAC_ETY	lcm_d = 1;
  for (j = 0; j < elts; j++)
    lcm_d = Lcm(lcm_d, row[j].D());
  if (lcm_d != 1) {
    for (j = 0; j < elts; j++) {
      // like row[j] *= lcm_d, but with less chance of overflow
      row[j] = FRAC((lcm_d/row[j].D()) * row[j].N());
    }
  }

  // make smallest integral
  FRAC_ETY	gcd_n = 1;
  for (j = 0; j < elts; j++)
    gcd_n = Gcd(gcd_n, row[j].N());
  if (gcd_n != 1) {
    for (j = 0; j < elts; j++) {
      // like row[j] /= gcd_n, but with less chance of overflow
      row[j] = FRAC(row[j].N() / gcd_n);
    }
  }
}
