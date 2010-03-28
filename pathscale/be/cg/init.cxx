/*
 *  Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
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


// This file contains only Linux-specific code and should be entirely
// #ifdef'd out for Irix.

// Work around the "undefined weak symbol" bug in Linux.
//
// see comments in be/com/weak.cxx.
//
// This file define initialization of pointer variables to symbols defined
// in cg.so but referenced in be/be.so.

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

#include "defs.h"
#include "cgdriver.h"
#include "cg.h"
#include "eh_region.h"
#ifdef TARG_X8664
#include "cgexp.h"
#include "calls.h"
#endif

// from be/cg/cgdriver.h
extern void (*CG_Init_p) ();
extern void (*CG_Fini_p) ();
extern void (*CG_Process_Command_Line_p) (INT, char **, INT, char **);

// from be/cg/cg.h
extern void (*CG_PU_Initialize_p) (WN*);
extern void (*CG_PU_Finalize_p) ();
extern void (*CG_Change_Elf_Symbol_To_Undefined_p) (ST*);
extern WN* (*CG_Generate_Code_p) (WN*, ALIAS_MANAGER*, DST_IDX, BOOL);
extern "C" void CG_Dump_Region(FILE*, WN*);
extern void (*CG_Dump_Region_p) (FILE*, WN*);

// from be/cg/eh_region.h
extern void (*EH_Generate_Range_List_p) (WN *);

#ifdef TARG_X8664
extern void (*CG_Set_Is_Stack_Used_p) ();
extern INT (*Push_Pop_Int_Saved_Regs_p) (void);
#endif

struct CG_INIT
{
    CG_INIT () {
	CG_Init_p = CG_Init;
	CG_Fini_p = CG_Fini;
	CG_Process_Command_Line_p = CG_Process_Command_Line;
	CG_PU_Initialize_p = CG_PU_Initialize;
	CG_PU_Finalize_p = CG_PU_Finalize;
	CG_Change_Elf_Symbol_To_Undefined_p = CG_Change_Elf_Symbol_To_Undefined;
	CG_Generate_Code_p = CG_Generate_Code;
	CG_Dump_Region_p = CG_Dump_Region;
	EH_Generate_Range_List_p = EH_Generate_Range_List;
#ifdef TARG_X8664
	CG_Set_Is_Stack_Used_p = CG_Set_Is_Stack_Used;
	Push_Pop_Int_Saved_Regs_p = Push_Pop_Int_Saved_Regs;
#endif
    }
} Cg_Initializer;

#endif // __linux__
