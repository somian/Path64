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

/** $Revision: 1.6 $
*** $Date: 04/12/21 14:57:14-08:00 $
*** $Author: bos@eng-25.internal.keyresearch.com $
*** $Source: /home/bos/bk/kpro64-pending/be/lno/SCCS/s.lu_mat_textra.cxx $
**/

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#define lu_mat_textra_CXX      "lu_mat_textra.cxx"
static const char *rcs_id =   lu_mat_textra_CXX "$Revision: 1.6 $";


#include "lnopt_main.h"
#include "mat.h"
#include "lu_mat.h"

template<>
void LU_DMAT::Print_Element(FILE* f, double e)
{
  fprintf(f, "%g", e);
}

template<>
void LU_FMAT::Print_Element(FILE* f, FRAC e)
{
  e.Print(f);
}

template<>
BOOL LU_FMAT::Exact_Arithmetic()
{
  return TRUE;
}

template<>
BOOL LU_DMAT::Exact_Arithmetic()
{
  return FALSE;
}

