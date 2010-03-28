//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_util.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  15-SEP-94 shin - Original Version
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:  Utilities for Optimizer
//
// ====================================================================
// ====================================================================


#ifndef opt_util_INCLUDED
#define opt_util_INCLUDED	"opt_util.h"
#ifdef _KEEP_RCS_ID
static char *opt_utilrcs_id = 	opt_util_INCLUDED"$Revision$";
#endif /* _KEEP_RCS_ID */

#ifndef srcpos_INCLUDED
#include "srcpos.h"
#endif /* srcpos_INCLUDED */
#ifndef opcode_gen_core_INCLUDED
#include "opcode_gen_core.h"
#endif

class BB_NODE;
class CODEREP;
class CODEMAP;

// warning for things to do
/*PRINTFLIKE1*/
extern void Warn_todo(const char*, ...)
#ifdef __GNUC__
        __attribute__((format(printf,1,2)))
#endif
	;

#ifndef opt_util_CXX
// TLOG interface for reporting optimizations
extern "C" void Opt_tlog( const char *keyword, SRCPOS srcpos, const char *fmt, ... );

extern "C" void Set_tlog_phase(const INT32 phase);
#endif


enum NUMBER_DESC {
  NUMBER_INVALID,   // not used (for error detection)
  NUMBER_OVERFLOW,   // value is overflowed
  NUMBER_UNKNOWN,   // value is unknown
  NUMBER_KNOWN      // value is determined
};

// 
class NUMBER {
  INT64       _value;
  NUMBER_DESC _desc;

public:

  NUMBER_DESC Desc(void) const    { return _desc; }
  INT64 Value(void) const         { return _value; }
  void  Set_desc(NUMBER_DESC n)   { _desc = n; }
  void  Set_value(INT64 value)    { _value = value; }

  void Init(void) 
    { Set_desc(NUMBER_INVALID);
      Set_value(0); }

  void Set_const(INT64 value) 
    { Set_desc(NUMBER_KNOWN); Set_value(value); }

  void Copy(NUMBER *n)
    { Set_desc(n->Desc()); Set_value(n->Value()); }

  void Eval1(OPERATOR opr, NUMBER *);
  void Eval2(OPERATOR opr, NUMBER *, NUMBER *);

  // The value can be represented using nbits
  BOOL Representable_in_nbits(INT32 nbits);
};

// the number of variants in an expression.
enum NUM_VARIANTS {
  NO_VARIANT    = 0,   // all are invariants
  ONE_VARIANT   = 1,   // contains 1 variant that is the same as vr
  NOT_ONE_VARIANT = 2,
     // contains more than 1 variant
     // contains some variant that is different from vr or more than 1 variant
     // contains opcodes other than  add/sub/neg/mul.
};

// Check if the expr is an induction expression of iv.
extern NUM_VARIANTS Find_one_variant(BB_NODE *bb,
                                     CODEREP *iv,
                                     CODEREP *expr,
                                     NUMBER *,
                                     CODEMAP *htable = NULL);

#if 1
#define SET_OPT_PHASE(str)  (void) Set_opt_phase(NULL, str)
#define SET_OPT_REPEAT_PHASE(phase_id, str) (void) Set_opt_phase(&phase_id, str)
INT Set_opt_phase(INT32 *, const char*);  // Set the current subphases of the optimizer
#define REPORT_STATISTICS() Report_statistics()
INT Report_statistics();   // Report statistics of subphases of the optimizer
#else
#define SET_OPT_PHASE(str)
#define REPORT_STATISTICS()
#endif

// Bitvector definitions
//
#include <vector>
#include "segmented_array.h" // For mempool_allocator

#ifdef __STL_USE_NAMESPACES
using std::vector;
#endif

typedef mempool_allocator<bool>         BVECTOR_ALLOCATOR;
typedef vector<bool, BVECTOR_ALLOCATOR> BVECTOR; // TODO add range checks

// For each coderep (i), determines whether it contains a reference
// to a volatile memory location or a volatile OPC, and if this is 
// the case sets vol[i]=TRUE.  
//
// PRECONDITION: For all i
//                 where: 0 <= i <= cfg->Htable()->Coderep_id_cnt()+1
//                 vol[i] == FALSE
//
class CFG;
void Set_volatile_map(CFG *cfg, BVECTOR &vol);

#endif  // opt_util_INCLUDED
