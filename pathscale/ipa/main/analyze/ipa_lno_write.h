/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#ifndef cxx_ipa_lno_write_INCLUDED
#define cxx_ipa_lno_write_INCLUDED 

#ifndef cxx_hash_INCLUDED
#include "cxx_hash.h"
#endif

#ifndef ipa_section_INCLUDED
#include "ipa_section.h" 
#endif

#ifndef ipa_lno_summary_INCLUDED
#include "ipa_lno_summary.h" 
#endif 

extern MEM_POOL IPA_LNO_mem_pool; 

typedef DYN_ARRAY<DESCR> DESCR_ARRAY; 
typedef DYN_ARRAY<IPA_LNO_SUMMARY_PROCEDURE> IPA_LNO_SUMMARY_PROCEDURE_ARRAY; 
typedef DYN_ARRAY<IPA_LNO_SUMMARY_FORMAL> IPA_LNO_SUMMARY_FORMAL_ARRAY; 
typedef DYN_ARRAY<IPA_LNO_SUMMARY_GLOBAL> IPA_LNO_SUMMARY_GLOBAL_ARRAY; 
typedef DYN_ARRAY<INT> INTEGER_ARRAY;
typedef HASH_TABLE<UINT64, INTEGER_ARRAY*> TERM_HASH_TABLE;

const INT IPA_LNO_TERM_HASH_TABLE_SIZE = 200; 

class IPA_LNO_WRITE_SUMMARY {
private: 
  MEM_POOL* _mem_pool; 
  IVAR_ARRAY* _ivar_array;
  IPA_LNO_SUMMARY_PROCEDURE_ARRAY* _procedure_array; 
  IPA_LNO_SUMMARY_FORMAL_ARRAY* _formal_array;  
  IPA_LNO_SUMMARY_GLOBAL_ARRAY* _global_array;  
  DYN_ARRAY<SUMMARY_VALUE>* _value_array;
  DYN_ARRAY<SUMMARY_EXPR>* _expr_array; 
  PROJECTED_REGION_ARRAY* _projected_region_array; 
  PROJECTED_ARRAY* _projected_node_array; 
  TERM_ARRAY* _term_array;
  TERM_HASH_TABLE *_term_hash_table;
 
public: 

  // Constructor 
  IPA_LNO_WRITE_SUMMARY(MEM_POOL* mem_pool) {
    _mem_pool = mem_pool;
    _ivar_array = CXX_NEW(IVAR_ARRAY(_mem_pool), _mem_pool);
    _procedure_array = CXX_NEW(IPA_LNO_SUMMARY_PROCEDURE_ARRAY(_mem_pool), 
      _mem_pool);
    _formal_array = CXX_NEW(IPA_LNO_SUMMARY_FORMAL_ARRAY(_mem_pool), 
      _mem_pool);
    _global_array = CXX_NEW(IPA_LNO_SUMMARY_GLOBAL_ARRAY(_mem_pool), 
      _mem_pool);
    _value_array = CXX_NEW(DYN_ARRAY<SUMMARY_VALUE>(_mem_pool), 
      _mem_pool);
    _expr_array = CXX_NEW(DYN_ARRAY<SUMMARY_EXPR>(_mem_pool), 
      _mem_pool);
    _projected_region_array = CXX_NEW(PROJECTED_REGION_ARRAY(_mem_pool), 
      _mem_pool);
    _projected_node_array = CXX_NEW(PROJECTED_ARRAY(_mem_pool), _mem_pool);
    _term_array = CXX_NEW(TERM_ARRAY(_mem_pool), _mem_pool);
    _term_hash_table = CXX_NEW(TERM_HASH_TABLE(IPA_LNO_TERM_HASH_TABLE_SIZE, 
      _mem_pool), _mem_pool);
  };

  // For memory pool 
  MEM_POOL* Mem_Pool() {return _mem_pool;};

  // For IVAR section
  IVAR_ARRAY* Ivar_Array() {return _ivar_array;};
  INT Ivar_Count() {return _ivar_array == NULL ? 0
    : (_ivar_array->Lastidx() + 1);};
  IVAR* Ivar(INT i) {return &(*_ivar_array)[i];};  

  // For PROCEDURE section 
  IPA_LNO_SUMMARY_PROCEDURE_ARRAY* Procedure_Array() 
    {return _procedure_array;};
  IPA_LNO_SUMMARY_PROCEDURE* Procedure(INT i) 
    {return &(*_procedure_array)[i];};
  INT Procedure_Count() {return _procedure_array == NULL ? 0
    : (_procedure_array->Lastidx() + 1);};

  // For FORMAL section 
  IPA_LNO_SUMMARY_FORMAL_ARRAY* Formal_Array() {return _formal_array;};
  IPA_LNO_SUMMARY_FORMAL* Formal(INT i) {return &(*_formal_array)[i];};
  INT Formal_Count() {return _formal_array == NULL ? 0
    : (_formal_array->Lastidx() + 1);};

  // For GLOBAL section 
  IPA_LNO_SUMMARY_GLOBAL_ARRAY* Global_Array() {return _global_array;};
  IPA_LNO_SUMMARY_GLOBAL* Global(INT i) {return &(*_global_array)[i];};
  INT Global_Count() {return _global_array == NULL ? 0
    : (_global_array->Elements());};

  // For PROJECTED REGION section 
  PROJECTED_REGION_ARRAY* Projected_Region_Array() 
    {return _projected_region_array;};
  PROJECTED_REGION* Projected_Region(INT i) 
    {return &(*_projected_region_array)[i];};
  INT Projected_Region_Count() {return _projected_region_array == NULL ? 0
    : (_projected_region_array->Lastidx() + 1);};

  // For PROJECTED (NODE) ARRAY section 
  PROJECTED_ARRAY* Projected_Node_Array() {return _projected_node_array;};
  PROJECTED_NODE* Projected_Node(INT i) {return &(*_projected_node_array)[i];};
  INT Projected_Node_Count() {return _projected_node_array == NULL ? 0
    : (_projected_node_array->Lastidx() + 1);};

  // For TERM ARRAY section 
  TERM_ARRAY* Term_Array() {return _term_array;};
  TERM* Term(INT i) {return &(*_term_array)[i];};
  INT Term_Count() {return _term_array == NULL ? 0 
    : (_term_array->Lastidx() + 1);};

  // For VALUE ARRAY section 
  DYN_ARRAY<SUMMARY_VALUE>* Value_Array() {return _value_array;};
  SUMMARY_VALUE* Value(INT i) {return &(*_value_array)[i];};
  INT Value_Count() {return _value_array == NULL ? 0 
    : (_value_array->Lastidx() + 1);};

  // For EXPR ARRAY section 
  DYN_ARRAY<SUMMARY_EXPR>* Expr_Array() {return _expr_array;};
  SUMMARY_EXPR* Expr(INT i) {return &(*_expr_array)[i];};
  INT Expr_Count() {return _expr_array == NULL ? 0 
    : (_expr_array->Lastidx() + 1);};

  // For TERM hash table
  TERM_HASH_TABLE* Term_Hash_Table() {return _term_hash_table;};
};

extern void IPA_LNO_Map_Node(IPA_NODE* node,
			     IPA_LNO_WRITE_SUMMARY* IPA_LNO_Summary);

extern void IPA_LNO_Write_Summary(IPA_LNO_WRITE_SUMMARY* IPA_LNO_Summary);

#endif // cxx_ipa_lno_write_INCLUDED
