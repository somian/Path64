//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_stmt.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  27-SEP-94 shin - Original Version
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
// Description:
//
// Declare classes STMT_LIST, STMTREP_ITER, and STMTREP_CONST_ITER
// to provide for lists of STMTREP nodes and for interation through
// these lists.
//
// ====================================================================
// ====================================================================


#ifndef opt_stmt_INCLUDED
#define opt_stmt_INCLUDED "opt_stmt.h"
#ifdef _KEEP_RCS_ID
static char *opt_stmtrcs_id = opt_stmt_INCLUDED"$ $Revision$";
#endif /* _KEEP_RCS_ID */


#ifndef defs_INCLUDED
#include "defs.h"
#endif
#ifndef cxx_base_INCLUDED
#include "cxx_base.h"
#endif


class STMT_CONTAINER;
class STMTREP;
class EMITTER;
class DU_MANAGER;


// ====================================================================


class STMT_LIST : public CHAIN {
  DECLARE_CHAIN_CLASS( STMT_LIST, STMTREP )
public:
  void     Print(FILE *fp = stderr) const;         // print out the STMT_LIST
  void     Gen_wn(STMT_CONTAINER *, EMITTER *);    // WN list from STMT_LIST
};


class STMTREP_ITER : public CHAIN_ITER {
  DECLARE_CHAIN_ITER_CLASS( STMTREP_ITER, STMTREP, STMT_LIST )
public:
  STMTREP_ITER(void)  {}
  ~STMTREP_ITER(void) {}
  void Init(void)     {}
  void Set_Cur(STMTREP *s) { CHAIN_ITER::Set_Cur((CHAIN_NODE *) s); }
};


class STMTREP_CONST_ITER : public CHAIN_ITER {
  DECLARE_CHAIN_CONST_ITER_CLASS( STMTREP_CONST_ITER, STMTREP, STMT_LIST )
public:
  STMTREP_CONST_ITER(void)  {}
  ~STMTREP_CONST_ITER(void) {}
  void Init(void)     {}
  void Set_Cur(STMTREP *s) { CHAIN_ITER::Set_Cur((CHAIN_NODE *) s); }
};


// ====================================================================


#endif  // opt_stmt_INCLUDED
