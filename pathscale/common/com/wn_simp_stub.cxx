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


/* This is a set of stubs for the WHIRL simplifier, for those
places where we don't want it. */
#ifdef USE_PCH
#include "common_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "defs.h"
#include "errors.h"
#include "erglob.h"
#include "tracing.h"
#include "stab.h"
#include "wn.h"
#include "wn_util.h"
#include "ir_reader.h"

#include "config.h"
#include "config_targ.h"

#include "const.h"
#include "targ_const.h"
#include "wn_simp.h"


BOOL WN_Simplifier_Enable(BOOL enable) 
{
   BOOL r = Enable_WN_Simp;
   Enable_WN_Simp = enable;
   return (r);
}

#if 0	/* mpm:  now defined in wn_simp? */
WN * WN_SimplifyExp1(OPCODE opc, MEM_POOL * pool, WN * k0)
{
   return (WN *) NULL;
}

WN * WN_SimplifyExp2(OPCODE opc, MEM_POOL * pool, WN * k0, WN * k1)
{
   return (WN *) NULL;
}

WN *WN_SimplifyCvtl(OPCODE opc, INT16 cvtl_bits, WN *k0)
{
   return (WN *) NULL;
}

#endif
