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


#ifndef w2c_weak_INCLUDED
#define w2c_weak_INCLUDED

// See comments in be/com/weak.cxx

// Symbols exported from whirl2c.so 
// and used in be, be.so and lno.so
// (additional ones may be needed for prompf_anl.so)

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

extern void (*W2C_Cleanup_p)(void);
extern void (*W2C_Init_p)(void);
extern void (*W2C_Outfile_Fini_p)(BOOL emit_global_decls);
extern void (*W2C_Outfile_Init_p)(BOOL emit_global_decls);
extern void (*W2C_Outfile_Translate_Pu_p)(WN *pu, BOOL emit_global_decls);
extern void (*W2C_Pop_PU_p)(void);
extern void (*W2C_Process_Command_Line_p)(INT phase_argc, const char * const
  phase_argv[], INT argc, const char * const argv[]);
extern void (*W2C_Push_PU_p)(const WN *pu, WN *body_part_of_interest);
extern void (*W2C_Set_Frequency_Map_p)(WN_MAP frequency_map);
extern void (*W2C_Set_Prompf_Emission_p)(const WN_MAP *construct_map);
extern BOOL (*W2C_Should_Emit_Nested_PUs_p)(void);
extern void (*W2C_Translate_Istore_Lhs_p)(char *strbuf,
  UINT bufsize, const WN* lhs, mINT64 istore_ofst, TY_IDX istore_addr_ty,
  TYPE_ID istore_mtype);
extern void (*W2C_Translate_Wn_p)(FILE *outfile, const WN *wn);

#define W2C_Cleanup (*W2C_Cleanup_p)
#define W2C_Init (*W2C_Init_p)
#define W2C_Outfile_Fini (*W2C_Outfile_Fini_p)
#define W2C_Outfile_Init (*W2C_Outfile_Init_p)
#define W2C_Outfile_Translate_Pu (*W2C_Outfile_Translate_Pu_p)
#define W2C_Pop_PU (*W2C_Pop_PU_p)
#define W2C_Process_Command_Line (*W2C_Process_Command_Line_p)
#define W2C_Push_PU (*W2C_Push_PU_p)
#define W2C_Set_Prompf_Emission (*W2C_Set_Prompf_Emission_p)
#define W2C_Set_Frequency_Map (*W2C_Set_Frequency_Map_p)
#define W2C_Should_Emit_Nested_PUs (*W2C_Should_Emit_Nested_PUs_p)
#define W2C_Translate_Istore_Lhs (*W2C_Translate_Istore_Lhs_p)
#define W2C_Translate_Wn (*W2C_Translate_Wn_p)

#else // __linux__

#pragma weak W2C_Cleanup
#pragma weak W2C_Init
#pragma weak W2C_Outfile_Fini
#pragma weak W2C_Outfile_Init
#pragma weak W2C_Outfile_Translate_Pu
#pragma weak W2C_Pop_PU
#pragma weak W2C_Process_Command_Line
#pragma weak W2C_Push_PU
#pragma weak W2C_Set_Frequency_Map
#pragma weak W2C_Set_Prompf_Emission
#pragma weak W2C_Should_Emit_Nested_PUs
#pragma weak W2C_Translate_Istore_Lhs
#pragma weak W2C_Translate_Wn
#pragma weak W2C_Translate_Wn_Str

#endif // __linux__

#endif // w2c_weak_INCLUDED
