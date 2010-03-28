//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_dce.h
// $Revision$
// $Date$
// $Author$
// $Source$
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


#ifndef opt_dce_INCLUDED
#define opt_dce_INCLUDED "opt_dce.h"

// ====================================================================
// Used to deal with redundant conditions
// ====================================================================

enum COND_EVAL {
  EVAL_UNINIT = 0,		// uninitialized
  EVAL_TRUE,		// condition evals to true
  EVAL_FALSE,		// condition evals to false
  EVAL_UNKNOWN,		// condition is unknown, but is init
  EVAL_DEAD		// condition is both true and false (used by
};			//   Eval_redundant_cond_br to indicate dead code)

#ifdef DEBUGGING_COND_EVAL
static 
const char *Cond_eval_name( COND_EVAL eval )
{
  switch ( eval ) {
    case EVAL_UNINIT : return "UNINIT";
    case EVAL_TRUE   : return "TRUE";
    case EVAL_FALSE  : return "FALSE";
    case EVAL_UNKNOWN: return "UNKNOWN";
    case EVAL_DEAD   : return "DEAD";
    default          : return "";
  }
}
#endif // DEBUGGING COND_EVAL

extern COND_EVAL Eval_redundant_cond_br( CODEREP *orig, CODEREP *evalcond, COND_EVAL eval );

#endif
