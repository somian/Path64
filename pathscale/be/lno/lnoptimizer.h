/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

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
 * Module: lnoptimizer.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  14-SEP-94 - Original Version
 *
 * Description:
 *
 * The external interface for the loop optimizer.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef lnoptimizer_INCLUDED
#define lnoptimizer_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "wn.h"
#include "mempool.h"

#ifdef __cplusplus
}
#endif

/* Clients of the loop nest optimizer pass a WHIRL tree for the function, 
 * and receive back a possibly optimized version of the tree.
 */
extern WN *Lnoptimizer(PU_Info* current_pu, WN *func_nd, struct DU_MANAGER *du_mgr, 
	struct  ALIAS_MANAGER *alias_mgr);

#endif /* lnoptimizer_INCLUDED */

