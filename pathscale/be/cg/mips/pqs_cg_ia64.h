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


// IA-64 specific declarations

#include "tn.h"
#include "op.h"
#include "bb.h"
#include "tn_map.h"
#include "op_map.h"
#include "cxx_memory.h"

extern TN_MAP PQS_tn_map;

extern PQS_NODE_IDX PQS_TN_get_last_definition(const TN *t);
extern void         PQS_TN_set_last_definition(const TN *t, PQS_NODE_IDX p);
extern BOOL         PQS_TN_used_as_qual_pred(const TN *t);
extern void         PQS_TN_set_used_as_qual_pred(const TN *t);
extern void         PQS_TN_set_no_query(const TN *t);
extern BOOL         PQS_TN_no_query(const TN *t);
extern TN *         PQS_TN_get_tn_to_use(const TN *t);
extern void         PQS_TN_set_tn_to_use(const TN *t, const TN *to_use);

extern OP_MAP PQS_op_map;
extern void PQS_OP_set_pqs_idx(OP *op, PQS_NODE_IDX p);
extern PQS_NODE_IDX PQS_OP_get_pqs_idx(OP *op);

extern PQS_ITYPE 
PQS_classify_instruction (OP *inst, TN * &qual, TN * &p1, TN * &p2, PQS_NODE_FLAGS &flags);
