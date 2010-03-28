/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2005 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
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
