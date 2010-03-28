/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: optimizer.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  14-SEP-94 - Original Version
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
// The external interface for the optimizer.
//
// ====================================================================
// ====================================================================


#ifndef optimizer_INCLUDED
#define optimizer_INCLUDED      "optimizer.h"
#ifdef _KEEP_RCS_ID
static char *optimizerrcs_id =	optimizer_INCLUDED"$Revision$";
#endif /* _KEEP_RCS_ID */

#include "opt_alias_interface.h"

/*  The phases of PREOPT */
typedef enum {
  PREOPT_PHASE,		// used for -PHASE:p
  PREOPT_LNO_PHASE,	// used for -PHASE:l
  PREOPT_LNO1_PHASE,	// used for -PHASE:l
  PREOPT_DUONLY_PHASE,  // called by LNO, but will disable optimization */
  MAINOPT_PHASE,	// used for -PHASE:w
  PREOPT_IPA0_PHASE,	// called by IPL
  PREOPT_IPA1_PHASE,	// called by main IPA
} PREOPT_PHASES;

typedef PREOPT_PHASES OPT_PHASE;

#ifdef __cplusplus
extern "C" {
#endif


/* Clients of the optimizer pass a WHIRL tree for the function, and
 * receive back a possibly optimized version of the tree.
 */
struct DU_MANAGER;

extern WN *Pre_Optimizer( INT32 /* PREOPT_PHASES */,  WN *, struct DU_MANAGER * , struct ALIAS_MANAGER *);

extern void choose_from_complete_struct_for_relayout_candidates();

struct DU_MANAGER* Create_Du_Manager(MEM_POOL *);
void               Delete_Du_Manager(struct DU_MANAGER *, MEM_POOL *);


#ifdef __cplusplus
}
#endif
#endif /* optimizer_INCLUDED */

