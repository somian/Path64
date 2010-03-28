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
 * Module: omp_lower.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  26-Jun-97 : First created by Dave Kohr
 *
 * Description:
 * Declarations of global functions from omp_lower.cxx
 *
 * Exported types:
 * None
 *
 * Exported functions and macros:
 * OMP_Prelower() : Perform first step of OpenMP pragma transformation
 * ==================================================================== */

#ifndef omp_lower_h_included
#define omp_lower_h_included

#ifdef __cplusplus
extern "C" {
#endif


#ifndef wn_INCLUDED
#include "wn.h"
#endif

#ifndef pu_info_INCLUDED
#include "pu_info.h"
#endif

extern WN* OMP_Prelower(PU_Info *current_pu, WN* pu);

#define THREAD_NUM_PREG_NAME "thread_num"


#ifdef __cplusplus
}
#endif

#ifndef symtab_INCLUDED
#include "symtab.h"
#endif

extern BOOL
WN_Store_Target_Matches_Reduction(WN *store, WN *reduction);

/*
The generalized lowerer for ATOMIC is available only in the OMP Prelowerer.
However, we want to be able to lower appropriate ATOMIC operations to
Compare-and-Swap and Fetch-And-Op from the MP Lowerer.  So the OMP
Prelowerer exports just enough of the ATOMIC implementation to allow the MP
Lowerer to identify and lower these kinds of ATOMIC.
*/

enum ATOMIC_Lowering_Class {
  ALCLASS_CRITICAL,
  ALCLASS_SWAP,
  ALCLASS_DIRECT,
  ALCLASS_ERROR
};

extern ATOMIC_Lowering_Class WN_ATOMIC_Lowering_Class(WN *atomic);

extern ATOMIC_Lowering_Class WN_ATOMIC_STORE_Lowering_Class(WN *store);

typedef void (*Update_Private_Func)(ST *st, WN *wn);

extern WN *
Atomic_Using_Swap(WN *atomic, WN *store, WN *operation, WN *parent,
                  Update_Private_Func upf, ST *x2 = NULL, ST *x3 = NULL);

extern WN *
Atomic_Direct(WN *atomic, WN *store, WN *operation);

extern WN *
Get_ATOMIC_Update_LDA(WN *wn);

#endif /* omp_lower_h_included */

