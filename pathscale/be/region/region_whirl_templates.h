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
//============================================================================
//
// Module: region_templates.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  1-SEP-97 dahl - Original Version
//
//============================================================================

// =======================================================================
// WHIRL search comparison functions for REGION_search_block
// can search two ways:
// 1) same pragma number in a pragma list
// 2) same label_number in a region exit list
struct comp_same_pragma {
 private:
  const WN_PRAGMA_ID _pragma;
 public:
  comp_same_pragma(const WN_PRAGMA_ID pragma) : _pragma(pragma) { }
  BOOL operator()(const WN *x) const {
    Is_True(WN_opcode(x) == OPC_PRAGMA,
	    ("comp_same_pragma, not a pragma"));
    if (WN_pragma(x) == _pragma)
      return TRUE;
    return FALSE;
  }
};

struct comp_same_label_no {
 private:
  const INT32 _label_number;
 public:
  comp_same_label_no(const INT32 label_no) : _label_number(label_no) { }
  BOOL operator()(const WN *x) const {
    // in CG these can be converted to gotos or labels, rest of the time they
    // are always region_exits
    Is_True(WN_opcode(x) == OPC_REGION_EXIT || WN_opcode(x) == OPC_LABEL ||
	    WN_opcode(x) == OPC_GOTO, 
	    ("comp_same_label_no, not a region exit or label"));
    return (WN_label_number(x) == _label_number);
  }
};

// =======================================================================
// REGION_search_block: search statements in a WHIRL block
// returns address of WHIRL node if found
template <class COMP>
WN *REGION_search_block(WN *block, COMP comp)
{
  Is_True(WN_opcode(block) == OPC_BLOCK, ("REGION_search_block, not a block"));
  for (WN *wtmp=WN_first(block); wtmp; wtmp=WN_next(wtmp))
    if (comp(wtmp))
      return wtmp;
  return NULL;
}
