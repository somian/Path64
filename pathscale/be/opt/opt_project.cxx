//-*-c++-*-

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

// ====================================================================
// ====================================================================
//
// Module: opt_project.cxx
// $Revision: 1.5 $
// $Date: 04/12/21 14:57:18-08:00 $
// $Author: bos@eng-25.internal.keyresearch.com $
// $Source: /home/bos/bk/kpro64-pending/be/opt/SCCS/s.opt_project.cxx $
//
// Revision history:
//  26-MAR-97 rkennedy - Original Version
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
//  Routines for special handling of operations that are "projectable"
//  in the sense of OPR_DIVREM.  See opt_project.h for details.
//
// ====================================================================
// ====================================================================


#include "erglob.h"
#include "opt_project.h"

// Given a CK_VAR coderep, find the RHS of a statement (if any) that
// whose value ultimately gets stored to that variable through a
// sequence of STID's whose LHS's are all EPRE_temp's that does not
// pass through any phi. If no such sequence exists, we return NULL.
STMTREP *
Proj_defstmt(const CODEREP *const var,
	     const OPT_STAB *const opt_stab)
{
  const CODEREP *rhs = var;
  STMTREP *def;

  do {
    // Because of copy-propagation and folding after SSAPRE, we can
    // see variables defined by chi assigned directly to EPRE
    // temporaries. In this case, this version of the EPRE temporary
    // cannot hold the result of a projectable operation.
    if (rhs->Is_flag_set(CF_DEF_BY_CHI) ||
        rhs->Is_flag_set(CF_IS_ZERO_VERSION) )
    {
      return NULL;
    }
    def = rhs->Defstmt();
    if (def != NULL) {
      if (opt_stab->Aux_stab_entry(def->Lhs()->Aux_id())->EPRE_temp()) {
	rhs = def->Rhs();
      }
      else {
	return NULL;
      }
    }
  } while ((def != NULL) &&
	   (rhs->Kind() == CK_VAR));
  if ((def != NULL) &&
      (rhs->Kind() == CK_OP)) {
    return def;
  }
  return NULL;
}
