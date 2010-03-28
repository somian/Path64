//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_tail.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  14-MAR-96 - Original Version
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
// ====================================================================
// ====================================================================


#ifndef opt_tail_INCLUDED
#define opt_tail_INCLUDED "opt_tail.h"


#ifdef _KEEP_RCS_ID
static char *opt_tail_rcs_id = opt_tail_INCLUDED" $Revision$";
#endif /* _KEEP_RCS_ID */


#include "defs.h"
#include "opt_cfg.h"
#include "opt_sym.h"
#include "wn.h"


//  OPT_TAIL: container for tail recursion optimization

class OPT_TAIL {
private:
  OPT_TAIL(void);
  OPT_TAIL(const OPT_TAIL&);
  OPT_TAIL& operator = (const OPT_TAIL&);

  BOOL		_do_trace;
  CFG 		*_cfg;
  OPT_STAB	*_opt_stab;
  BB_NODE	*_entry_bb;
  WN		*_entry_wn;
  WN		*_call_wn;
  WN		*_ret_ldid_wn;
  WN            *_ret_ldid_wn1;
  WN		*_ret_stid_wn;
  WN            *_ret_stid_wn1;
  BB_NODE	*_label_bb;
  WN		*_top_label;

protected:
  BOOL Entry_is_well_behaved();
  BOOL Exit_is_well_behaved(BB_NODE*);
  void Create_top_label();
  void Fixup_exit(BB_NODE*);

public:
  OPT_TAIL(CFG*, OPT_STAB*);
  ~OPT_TAIL();

  void Mutate();
};


#endif  // opt_tail_INCLUDED
