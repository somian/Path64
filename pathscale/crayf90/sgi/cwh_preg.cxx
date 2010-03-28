/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 * Module: cwh_preg.c
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:57:32-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/crayf90/sgi/SCCS/s.cwh_preg.cxx $
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: This file contains routines to provide pregs. 
 *              Copied from edwhirl.c, but later all the code to
 *              recycle pregs was removed.
 *
 * ====================================================================
 * ====================================================================
 */

static const char *source_file = __FILE__;

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: /home/bos/bk/kpro64-pending/crayf90/sgi/SCCS/s.cwh_preg.cxx $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */

/* sgi includes */

#include "defs.h" 
#include "stab.h"
#include "strtab.h"
#include "wn.h"

/* conversion removes */

#include "cwh_defines.h"
#include "cwh_preg.h"
#include "cwh_addr.h"



#define MTYPE_MAX MTYPE_V

static TYPE_ID preg_ty_typeid[MTYPE_MAX];
static TYPE_ID preg_bt_typeid[MTYPE_MAX];

/*=============================================
 *
 * cwh_preg_next_preg
 * 
 * Get the next preg of the given MTYPE.
 *
 *=============================================
 */
extern PREG_det
cwh_preg_next_preg (TYPE_ID    btype,
		    const char     * name,
		    WN       * home_wn )
{

  PREG_det det ;

  DevAssert(((btype < MTYPE_MAX) && (btype > MTYPE_FIRST)),
	    ("Missing mtype - 2")) ;
  
  det.preg_ty = Be_Type_Tbl(preg_ty_typeid[btype]) ;
  det.preg_st = MTYPE_To_PREG(preg_bt_typeid[btype]);
  det.preg    = Create_Preg (btype, name);
  
  return (det);
} 

/*=============================================
 *
 * cwh_preg_temp_save
 * 
 * Saves the given WN in a preg, whose type is
 * that of the expression, and returns a 
 * LDID of the preg. 
 *
 *=============================================
 */
extern WN *
cwh_preg_temp_save(const char * name, WN  * expr )
{
  TYPE_ID  bt;
  PREG_NUM pr;
  WN      *wn;
  
  bt = WNRTY(expr);
  pr = Create_Preg(bt,Index_To_Str(Save_Str(name)));
  cwh_addr_store_ST(MTYPE_To_PREG(bt), pr, 0, expr);

  wn = WN_LdidPreg(bt,pr);
  return wn;
}

/*=============================================
 *
 * fe_preg_init
 *
 * Set up preg tables.
 *
 *=============================================
 */
extern void
fe_preg_init (void)
{
  int i ;

  for (i = 0 ; i < MTYPE_MAX ; i++ ) {
    preg_ty_typeid[i] = MTYPE_I4 ;   
    preg_bt_typeid[i] = MTYPE_I4 ;
  }

  preg_ty_typeid[MTYPE_I8] = MTYPE_I8 ;   preg_bt_typeid[MTYPE_I8] = MTYPE_I8 ;

  preg_ty_typeid[MTYPE_U1] = MTYPE_U4 ;   
  preg_ty_typeid[MTYPE_U2] = MTYPE_U4 ;   
  preg_ty_typeid[MTYPE_U4] = MTYPE_U4 ;   
  preg_ty_typeid[MTYPE_U8] = MTYPE_U8 ;   preg_bt_typeid[MTYPE_U8] = MTYPE_I8 ;

  preg_ty_typeid[MTYPE_F4] = MTYPE_F4 ;   preg_bt_typeid[MTYPE_F4] = MTYPE_F4 ;
  preg_ty_typeid[MTYPE_F8] = MTYPE_F8 ;   preg_bt_typeid[MTYPE_F8] = MTYPE_F8 ;
  preg_ty_typeid[MTYPE_FQ] = MTYPE_FQ ;   preg_bt_typeid[MTYPE_FQ] = MTYPE_FQ ;

  preg_ty_typeid[MTYPE_C4] = MTYPE_F4 ;   preg_bt_typeid[MTYPE_C4] = MTYPE_F4 ;
  preg_ty_typeid[MTYPE_C8] = MTYPE_F8 ;   preg_bt_typeid[MTYPE_C8] = MTYPE_F8 ;
  preg_ty_typeid[MTYPE_CQ] = MTYPE_FQ ;   preg_bt_typeid[MTYPE_CQ] = MTYPE_FQ ;


} /* fe_preg_init */

