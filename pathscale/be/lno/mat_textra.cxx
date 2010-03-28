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
*** Matrix member functions and friends for which template functions are
*** not adequate.  Thus 'textra' => 'template extra'.
**/

/** $Revision: 1.6 $
*** $Date: 04/12/21 14:57:14-08:00 $
*** $Author: bos@eng-25.internal.keyresearch.com $
*** $Source: /home/bos/bk/kpro64-pending/be/lno/SCCS/s.mat_textra.cxx $
**/

#define mat_textra_CXX      "mat_textra.cxx"
static const char *rcs_id =   mat_textra_CXX "$Revision: 1.6 $";


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "lnopt_main.h"
#include "mat.h"
#include "lu_mat.h"

// ********************* Required for instantiation **********************

template <> MEM_POOL* MAT<mINT32>::_default_pool = NULL;
template <> MEM_POOL* MAT<FRAC>::_default_pool = NULL;
template <> MEM_POOL* MAT<double>::_default_pool = NULL;

// Integer matrix inversion can be performed by making the equivalent fraction
// matrix and inverting that.
// The alternative is to cast to doubles instead of fractions.
// That might well be faster, but may suffer from roundoff.  In either case,
// the routine crashes if the thing is not unimodular.
//
// The #if below chooses between the two.  #if 0 selects double rather than
// fractional inverse.

#if 0

IMAT MAT<mINT32>::Inv() const
{
  FmtAssert(_r == _c, ("Matrix not square"));

  MEM_POOL* hold = FMAT::Set_Default_Pool(IMAT::Default_Pool());
  FMAT fmat = IMAT_to_FMAT(*this);
  fmat.D_Inv();
  FMAT::Set_Default_Pool(hold);
  return FMAT_to_IMAT(fmat, IMAT::Default_Pool());
}

#else

template<>
IMAT MAT<mINT32>::Inv() const
{
  FmtAssert(_r == _c, ("Matrix not square"));

  MEM_POOL* hold = DMAT::Set_Default_Pool(IMAT::Default_Pool());
  DMAT dmat = IMAT_to_DMAT(*this, IMAT::Default_Pool());
  dmat.D_Inv();
  DMAT::Set_Default_Pool(hold);
  return DMAT_to_IMAT(dmat, IMAT::Default_Pool());
}

#endif

// Factor and use the factoring inversion routines

template<>
FMAT MAT<FRAC>::Inv() const		// XXX compiler bug: confused by FMAT
{
  FmtAssert(_r == _c, ("FMAT::Inv(): Matrix not square"));
  LU_FMAT lu(*this, FMAT::Default_Pool());
  return lu.Inv();
}

// Factor and use the factoring inversion routines

template<>
DMAT MAT<double>::Inv() const		// XXX compiler bug: confused by DMAT
{
  FmtAssert(_r == _c, ("Matrix not square"));
  LU_DMAT lu(*this, DMAT::Default_Pool());
  return lu.Inv();
}

template<>
void DMAT::Print_Element(FILE* f, double e)
{
  fprintf(f, "%g", e);
}

template<>
void IMAT::Print_Element(FILE* f, mINT32 e)
{
  fprintf(f, "%d", e);
}

template<>
void FMAT::Print_Element(FILE* f, FRAC e)
{
  e.Print(f);
}

//************** other MAT special functions ************

IMAT FMAT_to_IMAT(const FMAT& a, MEM_POOL* pool)
{
  Is_True(IMAT::Default_Pool(), ("Missing default pool for IMAT"));

  IMAT	x(a.Rows(), a.Cols(), pool);

  for (INT32 r = 0; r < a.Rows(); r++)
    for (INT32 c = 0; c < a.Cols(); c++)
      x(r,c) = a(r,c).Integer();

  return x;
}

IMAT DMAT_to_IMAT(const DMAT& a, MEM_POOL* pool)
{
  Is_True(IMAT::Default_Pool(), ("Missing default pool for IMAT"));

  IMAT	x(a.Rows(), a.Cols(), pool);

  for (INT32 r = 0; r < a.Rows(); r++) {
    for (INT32 c = 0; c < a.Cols(); c++) {
      double d = a(r, c);
      BOOL neg = (d < 0);
      if (neg) d = -d;
      INT32 i = INT32(d+0.5);
      FmtAssert(d-i < 1e-10 && d-i > -1e-10, ("Bad floating inverse"));
      x(r,c) = neg ? -i : i;
    }
  }

  return x;
}

DMAT IMAT_to_DMAT(const IMAT& a, MEM_POOL* pool)
{
  Is_True(DMAT::Default_Pool(), ("Missing default pool for DMAT"));

  DMAT	x(a.Rows(), a.Cols(), pool);

  for (INT32 r = 0; r < a.Rows(); r++)
    for (INT32 c = 0; c < a.Cols(); c++)
      x(r,c) = double(a(r,c));

  return x;
}

FMAT IMAT_to_FMAT(const IMAT& a, MEM_POOL* pool)
{
  Is_True(FMAT::Default_Pool(), ("Missing default pool for FMAT"));

  FMAT	x(a.Rows(), a.Cols(), pool);

  for (INT32 r = 0; r < a.Rows(); r++)
    for (INT32 c = 0; c < a.Cols(); c++)
      x(r,c) = FRAC(a(r,c));

  return x;
}

