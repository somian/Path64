/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
 */


/*******************************************************************//**
* \file cg_swp.h                                                       *
* \brief Basic SWP header file                                         *
***********************************************************************/

#ifndef cg_swp_INCLUDED
#define cg_swp_INCLUDED "cg_swp.h"

/* Software Pipelinig (SWP) related defines */
#define SWP_OPS_LIMIT    500
#define SWP_OPS_OVERHEAD 2

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "tn.h"
#include "cg_loop.h"

/* Standard Template Library (STL) related include files */
#include <vector>
#include <map>
#include <functional>


typedef CG_LOOP_BACKPATCH                         SWP_BP;

typedef mempool_allocator <INT>                   SWP_INT_ALLOC;
typedef mempool_allocator <mINT16>                SWP_mINT16_ALLOC;
typedef mempool_allocator <mUINT16>               SWP_mUINT16_ALLOC;
typedef mempool_allocator <float>                 SWP_FLOAT_ALLOC;
typedef mempool_allocator <BOOL>                  SWP_BOOL_ALLOC;
typedef mempool_allocator <TN *>                  SWP_TN_ALLOC;
typedef mempool_allocator <SWP_BP *>              SWP_BP_ALLOC;
typedef mempool_allocator <std::pair <TN *, 
                                      TN *> >     SWP_TN2TN_ALLOC;
typedef mempool_allocator <std::pair <const TN *, 
                                      INT> >      SWP_TN2INT_ALLOC;
typedef mempool_allocator <std::pair <SWP_BP *,
                                      BOOL> >     SWP_BP2BOOL_ALLOC;

typedef std::vector <INT, SWP_INT_ALLOC>          SWP_INT_VECTOR;
typedef std::vector <mINT16, SWP_mINT16_ALLOC>    SWP_mINT16_VECTOR;
typedef std::vector <mUINT16, SWP_mUINT16_ALLOC>  SWP_mUINT16_VECTOR;
typedef std::vector <float, SWP_FLOAT_ALLOC>      SWP_FLOAT_VECTOR;
typedef std::vector <BOOL, SWP_INT_ALLOC>         SWP_BOOL_VECTOR;
typedef std::vector <TN *, SWP_TN_ALLOC>          SWP_TN_VECTOR;
typedef std::vector <SWP_BP *, SWP_BP_ALLOC>      SWP_BP_VECTOR;
typedef std::map <TN *,
                  TN *,
                  std::less <TN *>,
                  SWP_TN2TN_ALLOC>                SWP_TN2TN_MAP;
typedef std::map <const TN *,
                  INT,
                  std::less <const TN *>,
                  SWP_TN2INT_ALLOC>               SWP_TN2INT_MAP;
typedef std::map <SWP_BP *,
                  BOOL,
                  std::less <SWP_BP *>,
                  SWP_BP2BOOL_ALLOC>              SWP_BP2BOOL_MAP;


enum SWP_LOOP_TYPE {
  SWP_Do_Loop,
  SWP_While_Loop,
};


enum SWP_STATUS_CODE {
  SWP_Multi_BB_Loop,
  SWP_Small_Trip_Count,
  SWP_Too_Large_MII,
  SWP_Scheduling_Failed,
  SWP_Register_Failed,
  SWP_Successful
};

#endif /* cg_swp_INCLUDED */

