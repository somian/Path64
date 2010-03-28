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


/* ====================================================================
 * ====================================================================
 *
 * Module: aux_stab.cxx
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * Auxiliary (i.e. BE-specific) symbol table routines.  In addition
 * to stubs for defining symbols which are defined by front-end
 * aux_stab.c and needed by com/stab.c, this module contains functions
 * for the following purposes:
 *
 *   Symbol table emission to support the debugger.
 *
 *   Stack frame reorganization and temporary allocation.
 *
 *   Listing and tracing of the symbol table contents.
 *
 * WARNING:  The restructuring of and additions to this module for
 * stack frame reorganization and symbol table listing/tracing has
 * ignored considerations of running in SINGLE_PROCESS mode.  Some work
 * will be necessary to resurrect that capability.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef SINGLE_PROCESS
/* If SINGLE_PROCESS, this file is included in *fe/aux_stab.c,
 * so don't do this.
 */

#include "defs.h"
#include "config_targ.h"
#include "config_asm.h"
#include "erglob.h"
#include "erbe.h"
#include "glob.h"
#include "tracing.h"
#include "mempool.h"

#include "mtypes.h"
#include "strtab.h"
#include "opcode.h"
#include "targ_const.h"
#include "stab.h"
#include "tn.h"
#include "bb.h"
#include "ttype.h"
#include "wn.h"
#include "const.h"
#endif /* SINGLE_PROCESS */

