/*
 * Copyright (C) 2005, 2007 PathScale Inc.  All Rights Reserved.
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

#ifndef MODULO_H
#define MODULO_H

#include "mrt.h"

class SCHED {
 private:
  SBB* bb;

  struct COST {
    unsigned int length;
    unsigned int MaxLive;
  } cost;

  ARRAY relaxed;
  void Setup_Ready_Queue();
  void Setup_Release_Deadline( int );

  // Modulo Reservation Table (MRT)
  MRT mrt;

  uint16_t sch_order;  // a unique scheduling order

  uint16_t _Lb;     // the min number of cycles for the flat schedule
  uint16_t _Ub;     // the max number of cycles for the flat schedule
  bool scheduled;  // is ddg scheduled successfully
  bool is_fwd;     // perform forward scheduling or backward scheduling

  // for MII
  static const int neg_inf = -9999;
  uint16_t _mii;
  uint16_t _rec_mii;
  uint16_t _res_mii;

  void Compute_ResMII();
  void Compute_RecMII();
  void Compute_MinDist();

  // for search space
  int  Compute_Critical_Path();
  void Compute_Scheduling_Ranges();
  ARRAY ready_queue;

  // for enumeration
  uint64_t budget;
  uint64_t tree_nodes;
  uint64_t tree_nodes_total;
  uint16_t stage_count;

  inline OPR* OPRs_Compare( OPR*, OPR* ) const;
  OPR* Select_Variable() const;
  bool schedule( OPR*, bool );
  bool Adjust_space_hazards( OPR* );

  bool Modulo_Scheduling();
  void List_Scheduling();

  void Perform_SWP();
  void Perform_LS();

  TALK talk;
  void Print( FILE* f = stdout ) const;

 public:
  SCHED( SBB*, TALK );

  void Emit_Schedule( BB * ) const;
  uint16_t Get_mii (void) const { return _mii; }
  uint16_t Get_rec_mii (void) const { return _rec_mii; }
  uint16_t Get_res_mii (void) const { return _res_mii; }
};

#endif
