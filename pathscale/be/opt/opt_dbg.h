//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_dbg.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  28-NOV-94 fchow - Original Version
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
//  This file provides things that are helpful inside dbx when
//  debugging WOPT.
//
// ====================================================================
// ====================================================================


#ifndef opt_dbg_INCLUDED
#define opt_dbg_INCLUDED     "opt_dbg.h"
#ifdef _KEEP_RCS_ID
static char *opt_dbgrcs_id = opt_dbg_INCLUDED"$Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef Is_True_On

class BB_NODE;
class CODEREP;
class STMTREP;

extern FILE *DFile;
extern COMP_UNIT *g_comp_unit;
extern OPT_STAB *g_opt_stab;

extern "C" void Dump_cfg(void);
extern "C" void Dump_bbs_reached(void);
extern "C" void Dump_bb(BB_NODE *bb);
extern "C" void Dump_cr(CODEREP *cr);
extern "C" void Dump_sr(STMTREP *sr);
extern "C" void show_cfg(CFG *cfg);

#endif	// Is_True_On
#endif  // opt_main_INCLUDED
