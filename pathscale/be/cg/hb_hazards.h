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


/* =======================================================================
 * =======================================================================
 *
 *  Module: hb_hazards.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Interface to the all hazard detection (machine hazards, bundling etc.)
 *  routines.
 *
 * =======================================================================
 * =======================================================================
 */
#ifndef hb_hazards_INCLUDED
#define hb_hazards_INCLUDED

#include "cgtarget.h"

// Creation and printing of scheduling notes.

typedef struct {
  INT schedule_length;
  INT block_parallelism;
  PRC_INFO prc_info;
  SRCPOS loop_srcpos;	// only valid for loop head BBs
} NOTE_SCHED;

inline OP*
BB_last_real_op(BB *bb) {
  OP *last_op;
  for (last_op = BB_last_op(bb); last_op; last_op = OP_prev(last_op)) {
    if (!OP_dummy(last_op)) break;
  }
  return last_op;
}

// Returns the last real OP preceeding <op>, i.e with ignoring all dummy OPs.
inline OP*
Last_Real_OP(OP *op)
{
  OP *last_real_op;
  for (last_real_op = OP_prev(op); last_real_op; last_real_op = OP_prev(last_real_op)) {
    if (!OP_dummy(last_real_op)) break;
  }

  return last_real_op;
}

extern void Handle_All_Hazards (BB *bb);
extern void Add_Scheduling_Notes_For_Loops ();

extern BOOL Is_Delay_Slot_Op (OP *op, BB *bb);
extern void Add_Scheduling_Note (BB *bb, void *bbsch);

#endif /* hb_hazards_INCLUDED */

