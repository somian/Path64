/*
 * Copyright 2003, 2004, 2005, 2006 PathScale Inc.  All Rights Reserved.
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

/* A local instruction scheduler dedicated to opteron.
   May 29, 2003
*/


#ifndef cg_sched_INCLUDED
#define cg_sched_INCLUDED "cg_sched.h"

#include "mempool.h"
#include "tn.h"
#include "cg_loop.h"
#include <stdint.h>


class KEY_SCH {
private:
  MEM_POOL* mem_pool;
  BB* bb;

  VECTOR _sched_vector;
  VECTOR _ready_vector;
  BOOL trace;

  OP* defop_by_reg[ISA_REGISTER_CLASS_MAX+1][REGISTER_MAX+1];
  int Addr_Generation( OP* );

  int _U;
  int _true_cp;
  int _cp;
  void Summary_BB();

  void Tighten_Release_Time( OP* );
  OP* last_mem_op;
  OP* Winner( OP*, OP* );
  OP* Select_Variable( int );

  void Init();
  void Build_OPR();
  void Build_Ready_Vector();
  void Schedule_BB();
  void Reorder_BB();

  // Sets of available registers in each register class.
  REGISTER_SET avail_reg_set[ISA_REGISTER_CLASS_MAX+1];

public:
  KEY_SCH( BB*, MEM_POOL*, BOOL );
  ~KEY_SCH() {};
}; 


#endif
