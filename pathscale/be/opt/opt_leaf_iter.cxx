//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_leaf_iter.cxx
// $Revision$
// $Date$
// $Author$
// $Source: /isms/cmplrs.src/osprey1.0/be/opt/RCS/opt_leaf_iter.cxx
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
// ====================================================================


#ifdef USE_PCH
#include "opt_pch.h"
#endif // USE_PCH
#pragma hdrstop


#include "opt_leaf_iter.h"

void
Expand_onto_stack(STACK<CODEREP *> &s, CODEREP *const cr)
{
  switch (cr->Kind()) {
  case CK_OP:
    { // Stupid scoping to avoid bogus warning about i.
      // Not a leaf; iterate over leaves of kids.
      for (mINT16 i = 0; i < cr->Kid_count(); ++i) {
	Expand_onto_stack(s, cr->Opnd(i));
      }
    }
    break;

  case CK_IVAR:
    // Not a leaf; iterate over leaves of LHS (if any) and RHS.
    if (cr->Ilod_base() != NULL) {
      Expand_onto_stack(s, cr->Ilod_base());
    }
    if (cr->Istr_base() != NULL) {
      Expand_onto_stack(s, cr->Istr_base());
    }
    break;

  case CK_VAR:
  case CK_LDA:
  case CK_CONST:
  case CK_RCONST:
    // A leaf. Iterate over this node.
    s.Push(cr);
    break;

  default:
    FmtAssert(FALSE,
	      ("CODEREP_KIDS_ITER::Expand_onto_stack: Bad Kind()"));
    break;
  }
}

void
Expand_onto_stack(STACK<WN *> &s, WN *const wn)
{
  Is_True(OPCODE_is_expression(WN_opcode(wn)),
	  ("Expand_onto_stack: WN_opcode must be expression op"));

  if (WN_kid_count(wn) == 0) {
    // A leaf. Iterate over this node.
    s.Push(wn);
  }
  else {
    // Not a leaf. Iterate over leaves of kids.
    for (mINT16 i = 0; i < WN_kid_count(wn); i++) {
      Expand_onto_stack(s, WN_kid(wn, i));
    }
  }
}

