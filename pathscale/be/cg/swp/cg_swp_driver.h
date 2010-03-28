/* 
 *  Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
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
* @file cg_swp_driver.h
* @brief Header file for the Software Pipelining (SWP) Driver.
*
* The Software Pipelining (SWP) Driver is the interface to the rest of 
* the compiler and controls the whole SWP process.
***********************************************************************/

#ifndef cg_swp_driver_INCLUDED
#define cg_swp_driver_INCLUDED "cg_swp_driver.h"

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "cg_loop.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_loop.h"
#include "cg_swp_op_vector.h"
#include "cg_swp_lifeness.h"
#include "cg_swp_lifetime.h"



/*******************************************************************//**
* @brief The Software Pieplining (SWP) Driver
*
* This class is just a control class to drive the Software Pipelining 
* (SWP) process.
* It also provides the MEM_POOL for the SWP process.
***********************************************************************/
class SWP_DRIVER : public CXX_MEM_POOL {
  private:
  /*****************************************************************//**
  * Mempool Type.
  * Defines the type of the mempool we want to create.
  *********************************************************************/
  enum SWP_MEM_POOL_TYPE {
    SWP_Zeroed     = TRUE, /**< Indicates that we want a zeroed 
                                mempool.*/
    SWP_Not_Zeroed = FALSE /**< Indicates that we don't want a zeroed 
                                mempool.*/
  };
  /********************************************************************/
  enum SWP_SEARCH_PHASE {
    SWP_Linear_Search,
    SWP_Exponential_Search,
    SWP_Binary_Search
  };

  MEM_POOL      *_m; /**< pointer to the local SWP mempool. */
  BB            *_body;
  OPS            _body_ops_backup;
  SWP_BP_VECTOR  _prologue_bp_backup;
  SWP_BP_VECTOR  _epilogue_bp_backup;

  static INT _loop_index; /**< current unique loop index. */
  
  void
  Rename_Dedicated_Registers (SWP_OP_VECTOR &ops);

  void
  Calculate_Prolog_Epilog_Stages (SWP_LOOP& swp_loop, 
                                  const SWP_OP_VECTOR& ops,
                                  const SWP_LIFETIME& lt);

  BOOL
  Is_Loop_Skipped (void) const;

  void
  Backup_Loop_Body_BB (void);
  void
  Restore_Loop_Body_BB (void);
  
  void
  Backup_Backpatches (void);
  void
  Restore_Backpatches (void);

  void
  Insert_Copy_For_Every_Same_Res_OP (void) const;

  public:
  SWP_DRIVER (void);
  ~SWP_DRIVER (void);

  BOOL 
  Perform_SWP (const CG_LOOP& loop, SWP_LOOP_TYPE loop_type);

  static void
  Print_Status_Message (LOOP_DESCR *loop, SWP_STATUS_CODE status); 
};

#endif /* cg_swp_driver_INCLUDED */

