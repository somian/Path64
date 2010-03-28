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


#ifndef dra_export_INCLUDED
#define dra_export_INCLUDED

/* ====================================================================
 * ====================================================================
 *
 * Module: dra_export.h
 *
 * Revision history:
 *  16-Jul-96: Original Version
 *
 * Description:
 *  Exported functions for DRA cloning and name mangling
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef defs_INCLUDED
#include "defs.h"               // standard definitions
#endif

#ifndef mtypes_INCLUDED
#include "mtypes.h"             // for TYPE_ID
#endif



class WN;

#ifdef __cplusplus
extern "C" {
#endif
extern BOOL Run_Dsm_Cloner;

extern BOOL Run_Dsm_Common_Check;

extern BOOL Run_Dsm_Check;

extern void DRA_Initialize (void);

extern void DRA_Processing(struct pu_info *pu_info,
                           WN *pu,
                           BOOL pu_has_feedback);

extern void DRA_Finalize (void);

#ifdef __cplusplus
}
#endif


extern ST *Find_Return_Registers (TYPE_ID type, 
                                        PREG_NUM *rreg1, 
                                        PREG_NUM *rreg2);

#endif
