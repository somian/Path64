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


// This file contains only Linux-specific code and should be entirely
// #ifdef'd out for Irix.

// Work around the "undefined weak symbol" bug in Linux.
//
// see comments in be/com/weak.cxx.
//
// This file define initialization of pointer variables to symbols defined
// in lno.so but referenced in be/be.so.

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

#include "defs.h"
#include "wn.h"
#include "pu_info.h"
#include "w2c_driver.h"

extern void (*W2C_Cleanup_p)(void);
extern void (*W2C_Fini_p)(void);
extern void (*W2C_Init_p)(void);
extern const char * (*W2C_Object_Name_p)(const ST *func_st);
extern void (*W2C_Mark_Loads_p)(void);
extern void (*W2C_Nomark_Loads_p)(void);
extern void (*W2C_Outfile_Fini_p)(BOOL emit_global_decls);
extern void (*W2C_Outfile_Init_p)(BOOL emit_global_decls);
extern void (*W2C_Outfile_Translate_Pu_p)(WN *pu, BOOL emit_global_decls);
extern void (*W2C_Pop_PU_p)(void);
extern void (*W2C_Process_Command_Line_p)(INT phase_argc, const char * const 
  phase_argv[], INT argc, const char * const argv[]);
extern void (*W2C_Push_PU_p)(const WN *pu, WN *body_part_of_interest);
extern void (*W2C_Set_Prompf_Emission_p)(const WN_MAP *construct_map);
extern void (*W2C_Set_Frequency_Map_p)(WN_MAP frequency_map);
extern const char *(*W2C_Get_Transformed_Src_Path_p)(void);
extern void (*W2C_Reset_Purple_Emission_p)(void);
extern void (*W2C_Set_Purple_Emission_p)(void);
extern BOOL (*W2C_Should_Emit_Nested_PUs_p)(void);
extern void (*W2C_Translate_Global_Defs_p)(FILE *outfile);
extern void (*W2C_Translate_Global_Types_p)(FILE *outfile);
extern void (*W2C_Translate_Istore_Lhs_p)(char *strbuf,
  UINT bufsize, const WN* lhs, mINT64 istore_ofst, TY_IDX istore_addr_ty,
  TYPE_ID istore_mtype);
extern void (*W2C_Translate_Purple_Main_p)(FILE *outfile, const WN *pu, 
  const char *region_name);
extern void (*W2C_Translate_Stid_Lhs_p)(char *strbuf, UINT bufsize,
  const ST *stid_st, mINT64 stid_ofst, TY_IDX stid_ty, TYPE_ID stid_mtype);
extern void (*W2C_Translate_Wn_p)(FILE *outfile, const WN *wn);
extern void (*W2C_Translate_Wn_Str_p)(char *strbuf, UINT bufsize, const
  WN *wn);

struct W2C_INIT
{
    W2C_INIT () {
      W2C_Cleanup_p = W2C_Cleanup;
      W2C_Fini_p = W2C_Fini;
      W2C_Init_p = W2C_Init;
      W2C_Object_Name_p = W2C_Object_Name;
      W2C_Mark_Loads_p = W2C_Mark_Loads;
      W2C_Nomark_Loads_p = W2C_Nomark_Loads;
      W2C_Outfile_Fini_p = W2C_Outfile_Fini;
      W2C_Outfile_Init_p = W2C_Outfile_Init;
      W2C_Outfile_Translate_Pu_p = W2C_Outfile_Translate_Pu;
      W2C_Pop_PU_p = W2C_Pop_PU;
      W2C_Process_Command_Line_p = W2C_Process_Command_Line;
      W2C_Push_PU_p = W2C_Push_PU;
      W2C_Set_Prompf_Emission_p = W2C_Set_Prompf_Emission;
      W2C_Set_Frequency_Map_p = W2C_Set_Frequency_Map;
      W2C_Get_Transformed_Src_Path_p = W2C_Get_Transformed_Src_Path;
      W2C_Reset_Purple_Emission_p = W2C_Reset_Purple_Emission;
      W2C_Set_Purple_Emission_p = W2C_Set_Purple_Emission;
      W2C_Should_Emit_Nested_PUs_p = W2C_Should_Emit_Nested_PUs;
      W2C_Translate_Global_Defs_p = W2C_Translate_Global_Defs;
      W2C_Translate_Global_Types_p = W2C_Translate_Global_Types;
      W2C_Translate_Istore_Lhs_p = W2C_Translate_Istore_Lhs;
      W2C_Translate_Purple_Main_p = W2C_Translate_Purple_Main;
      W2C_Translate_Stid_Lhs_p = W2C_Translate_Stid_Lhs;
      W2C_Translate_Wn_p = W2C_Translate_Wn;
      W2C_Translate_Wn_Str_p = W2C_Translate_Wn_Str;
    }
} W2C_Initializer;

#endif // __linux__
