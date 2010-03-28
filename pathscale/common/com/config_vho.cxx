/*
 *  Copyright (C) 2008, 2009 PathScale, LLC. All Rights Reserved.
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
 * Module: config_vho.c
 * $Revision: 1.16 $
 * $Date: 05/08/31 16:24:20-07:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: common/com/SCCS/s.config_vho.cxx $
 *
 * Revision history:
 *
 * Description:
 *
 * Configure the -VHO group (included in config.c).
 *
 * ====================================================================
 * ====================================================================
 */

/* This file is included in config.c, so it doesn't need its own set of
 * standard includes -- only the following:
 */
#include "config_vho.h"

/* ====================================================================
 *
 * Global flag variables which reflect the -VHO group options.
 *
 * ====================================================================
 */

#ifdef KEY
BOOL    VHO_Force_Lowering              = TRUE;
#else
BOOL    VHO_Force_Lowering              = FALSE;
#endif
BOOL    VHO_Struct_Opt                  = FALSE;
#ifdef KEY
BOOL    VHO_Struct_Opt_Set              = FALSE;
#endif
BOOL    VHO_Combine_Loads               = FALSE;
BOOL    VHO_Recycle_Pregs               = FALSE;
INT32   VHO_Switch_Density              = 12;
INT32   VHO_Switch_If_Else_Limit        = 6;
INT32   VHO_Switch_Compgoto_Limit       = 3;
BOOL    VHO_Switch_Opt                  = TRUE;
INT32   VHO_Switch_Opt_Threshold        = 25;
#ifdef KEY
BOOL    VHO_Switch_Reduce_Branch        = FALSE;
#endif
BOOL    VHO_Cselect_Opt                 = FALSE;
#ifdef KEY
BOOL    VHO_Cselect_Opt_Set             = FALSE;
#endif
BOOL    VHO_Iload_Opt                   = TRUE;
BOOL    VHO_Istore_Opt                  = TRUE;
BOOL    VHO_Call_Opt                    = FALSE;
#ifdef KEY
/* Turn icall to call conversion inside vho_lower to FALSE by default,
   because (1) the original implementation is buggy; and (2) we can now
   do icall to call conversion inside ipa.
*/
BOOL    VHO_Icall_Devir			= FALSE;
#else
BOOL    VHO_Icall_Devir			= TRUE;
#endif  // KEY
BOOL    VHO_Check_Tree                  = FALSE;
BOOL    VHO_Single_Loop_Test            = FALSE;
BOOL    VHO_Use_Do_While                = FALSE;
#ifdef KEY
/* simple if-conversion at VHO lower time */
BOOL    VHO_Enable_Simple_If_Conv = TRUE;   
/* maximum overhead allowed after If-Conversion */
INT32   VHO_Enable_If_Conv_Limit = 6;   
/* enable misc. loop distribution and interchange at VHO lower time */
BOOL    VHO_Enable_Misc_Loop_Transformation = TRUE;
/* enable misc. loop fusion at VHO lower time */
BOOL    VHO_Enable_Misc_Loop_Fusion = TRUE;
/* enable combining identical THEN or ELSE  parts in cascaded IFs */
BOOL    VHO_Merge_Thens = TRUE;
BOOL    VHO_Merge_Elses = FALSE;

/* Delete REGION_KIND_MP if region falls within these control bounds */
INT32 	VHO_Disable_MP_PU_Before = 0;
INT32 	VHO_Disable_MP_PU_After = 10000;
INT32 	VHO_Disable_MP_PU_Equal = 10000;
INT32 	VHO_Disable_MP_Local_Before = 0;
INT32 	VHO_Disable_MP_Local_After = 10000;
INT32 	VHO_Disable_MP_Local_Equal = 10000;

/* delete extra store of preg_return_val to a preg */
#ifdef TARG_MIPS
/* bug 14854, sct 5409 */
BOOL    VHO_Delete_Extra_Retval_Store = TRUE;
#else
BOOL    VHO_Delete_Extra_Retval_Store = FALSE;
#endif /* TARG_MIPS */
#endif /* KEY */

#ifdef TARG_X8664
BOOL	VHO_Generate_Rrotate = FALSE;
BOOL	VHO_Generate_Rrotate_Set = FALSE;
#endif

/* List of global variables to turn on and off various optimizations */

/* ====================================================================
 *
 * Descriptor for the -VHO option group.
 *
 * ====================================================================
 */

/* scalar optimizer options: */
static OPTION_DESC Options_VHO[] = {
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "lower",              "lower",
    FALSE, 0, 0, &VHO_Force_Lowering,  NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "struct_opt",         "struct_opt",
    TRUE, 0, 0,  &VHO_Struct_Opt,      &VHO_Struct_Opt_Set },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "recycle_pregs",      "recycle_pregs",
    FALSE, 0, 0, &VHO_Recycle_Pregs,   NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "combine_loads",      "combine_loads",
    FALSE, 0, 0, &VHO_Combine_Loads,   NULL },
  { OVK_INT32,	OV_INTERNAL,	FALSE, "switch_if_else",      "switch_if_else",
    6, 1, INT32_MAX,    &VHO_Switch_If_Else_Limit,   NULL },
  { OVK_INT32,	OV_INTERNAL,	FALSE, "switch_density",      "switch_density",
    12, 1, INT32_MAX,    &VHO_Switch_Density,   NULL },
  { OVK_INT32,	OV_INTERNAL,	FALSE, "switch_compgoto",     "switch_compgoto",
    3, 1, INT32_MAX,    &VHO_Switch_Compgoto_Limit,   NULL },
  { OVK_INT32,  OV_INTERNAL,    FALSE, "switch_opt_threshold", "switch_opt_threshold",
    10, 1, INT32_MAX,    &VHO_Switch_Opt_Threshold,   NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "switch_opt",         "switch",
    TRUE, 0, 0,  &VHO_Switch_Opt,      NULL },
#ifdef KEY
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "switch_reduce_branch", "switch_reduce_branch",
    TRUE, 0, 0,  &VHO_Switch_Reduce_Branch,      NULL },
#endif
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "cselect_opt",        "cselect",
    TRUE, 0, 0,  &VHO_Cselect_Opt,      &VHO_Cselect_Opt_Set },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "iload_opt",          "iload",
    TRUE, 0, 0,  &VHO_Iload_Opt,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "istore_opt",         "istore",
    TRUE, 0, 0,  &VHO_Istore_Opt,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "call_opt",           "call",
    TRUE, 0, 0,  &VHO_Call_Opt,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	TRUE, "icall_devir",          "icall",
    TRUE, 0, 0,  &VHO_Icall_Devir,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "check_tree",         "check_tree",
    TRUE, 0, 0,  &VHO_Check_Tree,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "single_loop_test",   "single_loop_test",
    TRUE, 0, 0,  &VHO_Single_Loop_Test,      NULL },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "use_do_while",       "use_do_while",
    TRUE, 0, 0,  &VHO_Use_Do_While,      NULL },
#ifdef KEY
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "if_conv",                "",
    0, 0, 0,    &VHO_Enable_Simple_If_Conv, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "ifconv_limit",           "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Enable_If_Conv_Limit, NULL },
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "misc_loop_transformation", "",
    0, 0, 0,    &VHO_Enable_Misc_Loop_Transformation, NULL },
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "misc_loop_fusion", "",
    0, 0, 0,    &VHO_Enable_Misc_Loop_Fusion, NULL },
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "merge_thens", "",
    0, 0, 0,    &VHO_Merge_Thens, NULL },
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "merge_elses", "",
    0, 0, 0,    &VHO_Merge_Elses, NULL },

  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_pu_before", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_PU_Before, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_pu_after", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_PU_After, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_pu_equal", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_PU_Equal, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_local_before", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_Local_Before, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_local_after", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_Local_After, NULL },
  { OVK_INT32,  OV_INTERNAL,    TRUE, "disable_mp_local_equal", "",
    INT32_MAX, 0, INT32_MAX,    &VHO_Disable_MP_Local_Equal, NULL },

  { OVK_BOOL,   OV_INTERNAL,    TRUE, "del_extra_retval_store", "",
    0, 0, 0,    &VHO_Delete_Extra_Retval_Store, NULL },
#endif
#ifdef TARG_X8664
  { OVK_BOOL,   OV_INTERNAL,    TRUE, "rotate", "",
    0, 0, 0,    &VHO_Generate_Rrotate, &VHO_Generate_Rrotate_Set },
#endif
  { OVK_COUNT }		/* List terminator -- must be last */
};
