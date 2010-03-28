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


#ifndef INNER_FISSION_RCS_ID
#define INNER_FISSION_RCS_ID
#ifdef _KEEP_RCS_ID
static char *inner_fission_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */
#endif

#ifndef _inner_fission_INCLUDED
#define _inner_fission_INCLUDED

#include "defs.h"
#include "dep_graph.h"
#include "wn.h"
#include "btree.h"
#include "lno_bv.h"
#include "name.h"

extern WN* Find_Stmt_Under(WN* stmt,WN* body);

extern void Inner_Fission(WN* func_nd,
                   ARRAY_DIRECTED_GRAPH16 *array_dependence_graph);

extern INT Fission_Inner_Loop(WN* innerloop,
BOOL known_to_have_register_allocation_problem=TRUE);

typedef enum { NOT_INCLUDED=0, IS_SCALAR=1, IS_ARRAY=2 } REF_CLASS;
typedef HASH_TABLE<WN*,REF_CLASS> WN2REF_CLASS;

extern  UINT inner_fission_2(
	WN* loop,		// enclosing loop
	SCALAR_STACK* scalar_reads,	// read refs to be examined
	SCALAR_STACK* scalar_writes,	// write refs to be examined
	REF_LIST_STACK* reads,	// read refs to be examined
	REF_LIST_STACK* writes,	// write refs to be examined
	BINARY_TREE<NAME2BIT> *mapping_dictionary,
		// dictionary to be updated which records mapping from
		// symbol names to bit positions
	FF_STMT_LIST& expandable_ref_list,
		// list contains all expandable refs after inner_fission_2
        MEM_POOL* mpool);

extern void Register_Name_To_Statement(
	WN* loop,		// enclosing loop
	SCALAR_STACK* scalar_reads,	// read refs to be examined
	SCALAR_STACK* scalar_writes,	// write refs to be examined
	REF_LIST_STACK* reads,	// read refs to be examined
	REF_LIST_STACK* writes,	// write refs to be examined
	HASH_TABLE<WN*,UINT>* stmt_id,
		// hash table which maps a statement to a integer statement id
	BIT_VECTOR* stmt_name_set,
		// name set bit vector array for all statements
	BINARY_TREE<NAME2BIT> *bit_pos_mapping);
		// dictionary which records mapping from
		// symbol names to bit positions

#endif



