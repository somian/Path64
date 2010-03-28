/*
 * Copyright (C) 2007, 2008. PathScale, LLC. All Rights Reserved.
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


//-*-c++-*-
//-----------------------------------------------------------------------------
//    CALL_INFO
//              Contains information about CALL statement
//
//		the scalar/array access info is summrized in the same kind
//		of data structure as the one used for ARA_LOOP_INFO.
//
//		The plan is to construct this info using results from
//		IPA analysis. For now, for the purpose of testing, we
//		allow a special subroutine call such as
//
//			call lno_test(a,'w',2,4,1,1,n,2)
//
//		to imitate a write access to a(2:4:1,1:n:2)
//      
//      CALL_INFO(MEM_POOL *pool) 
//	CALL_INFO(ARA_LOOP_INFO* ara_call, MEM_POOL *pool)
//	CALL_INFO(CALL_INFO *ci, MEM_POOL *pool)
//
//		constructors
//
//      MEM_POOL *Pool()
//
//              Which pool was used for this info
//
//      void Print(FILE *fp, BOOL terse = FALSE)
//
//              Print out the info.  If terse, don't print parts of the 
//              CALL_INFO that have no entries.
//
//	void Set_Call_Ara_Info(ARA_LOOP_INFO *a)
//
//		Assign array summary info
//
//	ARA_LOOP_INFO * Call_Ara_Info(void) const
//
//		Get array summary info
//
//	extern void Call_Info_Walk(WN* root)
//
//		Walk the tree and generate CALL_INFO for calls to
//		subroutines prefixed with "lno_test".
//
//	inline void Set_Call_Info(WN* wn, CALL_INFO* cli)
//	inline BOOL Has_Call_Info(WN* wn)
//	inline CALL_INFO* Get_Call_Info(WN* wn)
//
//		Set, query and get CALL_INFO for a call stmt
//
//	extern void Process_Call(WN* wn)
//
//		Process the call stmt to produce appropriate CALL_INFO.
//
//-----------------------------------------------------------------------------

#ifndef call_info_INCLUDED
#define call_info_INCLUDED

#include <sys/types.h>
#if defined(BUILD_OS_DARWIN)
#include <darwin_elf.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#ifndef _ara_loop_info_INCLUDED
#include "ara_loop.h"
#endif
#ifndef access_vector_INCLUDED
#include "access_vector.h"
#endif
#ifndef ipl_summary_INCLUDED
#include "ipl_summary.h"
#endif

class ARA_LOOP_INFO; 

class CALL_INFO
{
  BOOL _is_evaluated; 
  BOOL _needs_evaluation;
  BOOL _has_ipa_summaries; 
  WN* _wn_call;
  MEM_POOL* _pool;
  ARA_LOOP_INFO* _ara_call;
  ARA_LOOP_INFO* _ara_call_save;
  DYN_ARRAY<SUMMARY_VALUE>* _value; 
  DYN_ARRAY<SUMMARY_EXPR>* _expr; 


public:

  CALL_INFO(MEM_POOL *pool) { 
    _needs_evaluation = TRUE; 
    _is_evaluated = FALSE; 
    _pool = pool;
    _ara_call = NULL;
    _ara_call_save = NULL;
    _wn_call = NULL;
    _value = NULL;
    _expr = NULL; 
  }

  CALL_INFO(ARA_LOOP_INFO* ara_call, WN* wn_call, BOOL has_ipa_summaries, 
	MEM_POOL *pool) { 
    _needs_evaluation = TRUE; 
    _is_evaluated = FALSE; 
    _has_ipa_summaries = has_ipa_summaries; 
    _pool = pool;
    _ara_call = ara_call;
    _ara_call_save = NULL;
    _wn_call = wn_call;
    _value = NULL; 
    _expr = NULL; 
  }

  CALL_INFO(CALL_INFO *ci);
  MEM_POOL *Pool() { return _pool; };
  void Print(FILE *fp);
  void Tlog_Print();

  // member access functions
  void Set_Call_Ara_Info(ARA_LOOP_INFO *a){ _ara_call = a;}
  ARA_LOOP_INFO * Call_Ara_Info(void) const { return _ara_call; }
  void Evaluate();
  void Unevaluate();
  BOOL Has_Formal_Parameter();
  void Reset_Needs_Evaluation() {_needs_evaluation = FALSE;}
  void Set_Needs_Evaluation() {_needs_evaluation = TRUE;}
  BOOL Has_IPA_Summaries() {return _has_ipa_summaries;}

  // For execution cost analysis
  void Set_Value(DYN_ARRAY<SUMMARY_VALUE>* sv) {_value = sv;}
  DYN_ARRAY<SUMMARY_VALUE>* Value() const {return _value;}
  void Set_Expr(DYN_ARRAY<SUMMARY_EXPR>* sx) {_expr = sx;}
  DYN_ARRAY<SUMMARY_EXPR>* Expr() const {return _expr;}

};

extern void Call_Info_Walk(WN* root);

inline void Set_Call_Info(WN* wn, CALL_INFO* cli)
{
  Is_True(wn && WN_operator(wn) == OPR_CALL,
	("Set_Call_Info bug"));
  WN_MAP_Set(LNO_Info_Map, wn, cli);
}

inline BOOL Has_Call_Info(WN* wn) {
  return ((CALL_INFO*)WN_MAP_Get(LNO_Info_Map, wn)!=NULL);
}

inline BOOL Has_Execution_Cost(WN* wn) {
  CALL_INFO* call_info = (CALL_INFO*) WN_MAP_Get(LNO_Info_Map, wn);
  if (call_info != NULL) 
    return call_info->Value() != NULL || call_info->Expr();
  return FALSE;  
}

inline CALL_INFO* Get_Call_Info(WN* wn)
{
  Is_True(wn && WN_operator(wn) == OPR_CALL,
	("Get_Call_Info bug"));
  Is_True(Has_Call_Info(wn), ("Missing CALL_INFO"));

  return (CALL_INFO*)WN_MAP_Get(LNO_Info_Map, wn);
}

extern void Process_Call(WN* wn);	// produce CALL_INFO for call stmt 'wn'

#endif
