/*
 *  Copyright (C) 2008 PathScale, LLC. All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 * Module: config_vho.h
 * $Revision: 1.11 $
 * $Date: 05/08/31 16:24:20-07:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: common/com/SCCS/s.config_vho.h $
 *
 * Revision history:
 *  05-May-96 - Extracted from be/opt/opt_config.h.
 *
 * Description:
 *
 * Declare global flag variables for -VHO group options.
 * This file is included in common/com/config.c, but should not be
 * otherwise used outside of VHO.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef config_vho_INCLUDED
#define config_vho_INCLUDED

extern BOOL  VHO_Force_Lowering;
extern BOOL  VHO_Struct_Opt;
#ifdef KEY
extern BOOL  VHO_Struct_Opt_Set;
#endif
extern BOOL  VHO_Recycle_Pregs;
extern BOOL  VHO_Combine_Loads;
extern INT32 VHO_Switch_Density;
extern INT32 VHO_Switch_If_Else_Limit;
extern INT32 VHO_Switch_Compgoto_Limit;
extern BOOL  VHO_Switch_Opt;
extern INT32 VHO_Switch_Opt_Threshold;
#ifdef KEY
extern BOOL  VHO_Switch_Reduce_Branch;
#endif
extern BOOL  VHO_Cselect_Opt;
extern BOOL  VHO_Iload_Opt;
extern BOOL  VHO_Istore_Opt;
extern BOOL  VHO_Call_Opt;
extern BOOL  VHO_Icall_Devir;
extern BOOL  VHO_Check_Tree;
extern BOOL  VHO_Single_Loop_Test;
extern BOOL  VHO_Use_Do_While;
#ifdef KEY
extern BOOL  VHO_Enable_Simple_If_Conv; // Enable simple if-conv at VHO lowering
extern INT32 VHO_Enable_If_Conv_Limit;  // Maximum overhead allowed after ifconv
extern BOOL  VHO_Enable_Misc_Loop_Transformation; // Enable loop transformation
extern BOOL  VHO_Enable_Misc_Loop_Fusion; // Enable loop fusion
extern BOOL  VHO_Merge_Thens; 	   // enable combining identical THEN parts
				   // in cascaded IFs
extern BOOL  VHO_Merge_Elses; 	   // enable combining identical ELSE parts
				   // in cascaded IFs

extern INT32 VHO_Disable_MP_PU_Before;
extern INT32 VHO_Disable_MP_PU_After;
extern INT32 VHO_Disable_MP_PU_Equal;
extern INT32 VHO_Disable_MP_Local_Before;
extern INT32 VHO_Disable_MP_Local_After;
extern INT32 VHO_Disable_MP_Local_Equal;

extern BOOL  VHO_Delete_Extra_Retval_Store; // delete extra store of
                                            // preg_return_val to a preg
#endif
#ifdef TARG_X8664
extern BOOL  VHO_Generate_Rrotate;
extern BOOL  VHO_Generate_Rrotate_Set;
#endif
#endif /* config_vho_INCLUDED */

