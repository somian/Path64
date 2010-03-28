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
#include "w2f_driver.h"

extern void (*W2F_Cleanup_p)(void);
// extern void (*W2F_Def_ST_p)(FILE *outfile, ST *st);
extern void (*W2F_Fini_p)(void);
extern void (*W2F_Init_p)(void);
extern void (*W2F_Mark_Loads_p)(void);
extern void (*W2F_Nomark_Loads_p)(void);
extern const char * (*W2F_Object_Name_p)(ST *func_st);
extern void (*W2F_Outfile_Fini_p)(void);
extern void (*W2F_Outfile_Init_p)(void);
extern void (*W2F_Outfile_Translate_Pu_p)(WN *pu);
extern void (*W2F_Pop_PU_p)(void);
extern void (*W2F_Process_Command_Line_p)(INT phase_argc, const char *phase_argv[], 
  INT argc, const char *argv[]);
extern void (*W2F_Push_PU_p)(WN *pu, WN *body_part_of_interest);
extern void (*W2F_Set_Prompf_Emission_p)(WN_MAP *construct_map);
extern void (*W2F_Set_Frequency_Map_p)(WN_MAP frequency_map);
extern const char * (*W2F_Get_Transformed_Src_Path_p)(void);
extern void (*W2F_Reset_Purple_Emission_p)(void);
extern void (*W2F_Set_Purple_Emission_p)(void);
extern BOOL (*W2F_Should_Emit_Nested_PUs_p)(void);
extern void (*W2F_Translate_Istore_Lhs_p)(char *strbuf, UINT bufsize,
  WN* lhs, mINT64 istore_ofst, TY_IDX istore_addr_ty, TYPE_ID istore_mtype);
extern void (*W2F_Translate_Purple_Main_p)(FILE *outfile, WN *pu, 
  const char *region_name);
extern void (*W2F_Translate_Stid_Lhs_p)(char *strbuf, UINT bufsize, 
  ST *stid_st, mINT64 stid_ofst, TY_IDX stid_ty, TYPE_ID stid_mtype);
extern void (*W2F_Translate_Wn_p)(FILE *outfile, WN *wn);
extern void (*W2F_Translate_Wn_Str_p)(char *strbuf, UINT bufsize, WN *wn);

struct W2F_INIT
{
    W2F_INIT () {
      W2F_Cleanup_p = W2F_Cleanup;
      // W2F_Def_ST_p = W2F_Def_ST;
      W2F_Fini_p = W2F_Fini;
      W2F_Init_p = W2F_Init;
      W2F_Mark_Loads_p = W2F_Mark_Loads;
      W2F_Nomark_Loads_p = W2F_Nomark_Loads;
      W2F_Object_Name_p = W2F_Object_Name;
      W2F_Outfile_Fini_p = W2F_Outfile_Fini;
      W2F_Outfile_Init_p = W2F_Outfile_Init;
      W2F_Outfile_Translate_Pu_p = W2F_Outfile_Translate_Pu;
      W2F_Pop_PU_p = W2F_Pop_PU;
      W2F_Process_Command_Line_p = W2F_Process_Command_Line;
      W2F_Push_PU_p = W2F_Push_PU;
      W2F_Set_Prompf_Emission_p = W2F_Set_Prompf_Emission;
      W2F_Set_Frequency_Map_p = W2F_Set_Frequency_Map;
      W2F_Get_Transformed_Src_Path_p = W2F_Get_Transformed_Src_Path;
      W2F_Reset_Purple_Emission_p = W2F_Reset_Purple_Emission;
      W2F_Set_Purple_Emission_p = W2F_Set_Purple_Emission;
      W2F_Should_Emit_Nested_PUs_p = W2F_Should_Emit_Nested_PUs;
      W2F_Translate_Istore_Lhs_p = W2F_Translate_Istore_Lhs;
      W2F_Translate_Purple_Main_p = W2F_Translate_Purple_Main;
      W2F_Translate_Stid_Lhs_p = W2F_Translate_Stid_Lhs;
      W2F_Translate_Wn_p = W2F_Translate_Wn;
      W2F_Translate_Wn_Str_p = W2F_Translate_Wn_Str;
    }
} W2F_Initializer;

#endif // __linux__
