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


//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: cg_swp_bundle.h
//
//   SWP_Bundle: This function will bundle the ops in the op_state,
//   denoting each SWP_OP with a slot number offset from the beginning
//   of the loop body.  Each OP will be marked as OP_end_group() and
//   OP_m_unit as appropriate, and every op will be marked as OP_bundled.
//   New noops will be inserted where necessary to provide legal bundling.
//
//   The noops are inserted at the end of the op_state, and we assume
//   the ops will be sorted on slot position by the emitter to put them
//   in the final bundled order.
// ====================================================================
// ====================================================================
//

#ifndef cg_swp_bundle_INCLUDED
#define cg_swp_bundle_INCLUDED "cg_swp_bundle.h"


void SWP_Bundle(SWP_OP_vector& op_state, bool trace);

void SWP_Dont_Bundle(SWP_OP_vector& op_state);

void SWP_Undo_Bundle(SWP_OP_vector& op_state, BB *body);

#endif // cg_swp_bundle_INCLUDED


