/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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



// whether to use the base st for the reloc.
extern BOOL CGEMIT_Use_Base_ST_For_Reloc (INT reloc, ST *st);

// put reloc info into asm buffer, returning # parens to close with.
extern INT CGEMIT_Relocs_In_Asm (TN *t, ST *st, vstring *buf, INT64 *val);
// put reloc info into object file.
extern void CGEMIT_Relocs_In_Object (
	TN *t, ST *st, INT32 PC, pSCNINFO PU_section, INT64 *val);

// add events and relocs as needed for call.
extern void CGEMIT_Add_Call_Information (
	OP *op, BB *bb, INT32 PC, pSCNINFO PU_section);
#if defined(TARG_MIPS) || defined(TARG_X8664)
extern void
CGEMIT_Prn_Scn_In_Asm (ST *st, ST *cur_section);
extern void CGEMIT_Change_Origin_In_Asm (ST *st, INT64 offset);
#else
extern void
CGEMIT_Prn_Scn_In_Asm (ST *st, Elf64_Word scn_type, Elf64_Word scn_flags,
		       Elf64_Xword scn_entsize, ST *cur_section);
#endif
extern void
CGEMIT_Prn_Scn_In_Asm (FILE       *asm_file,
		       const char *scn_name,
		       Elf64_Word  scn_type,
		       Elf64_Word  scn_flags,
		       Elf64_Xword scn_entsize,
		       Elf64_Word  scn_align,
		       const char *cur_scn_name);
extern void
CGEMIT_Prn_Line_Dir_In_Asm (USRCPOS usrcpos);
extern void
CGEMIT_Prn_File_Dir_In_Asm(USRCPOS usrcpos,
                        const char *pathname,
                        const char *filename);

// Generate the .frame, .mask and the .fmask directives for the assembler.
extern void CGEMIT_Gen_Asm_Frame (INT64 frame_len);

// Prepass before emit to fix up fp registers for IA-32.
// Noop for every other target.
extern void STACK_FP_Fixup_PU();

// Generate the entry (.proc) directive.
extern void CGEMIT_Prn_Ent_In_Asm (ST *pu);

// generate weak alias directive.
extern void CGEMIT_Weak_Alias (ST *sym, ST *strongsym);

// generate alias directive.
extern void CGEMIT_Alias (ST *sym, ST *strongsym);

#ifdef KEY
extern INT CGEMIT_Print_Inst( OP* op, const char* result[], const char* opnd[], FILE* f );
extern void CGEMIT_Setup_Ctrl_Register( FILE* f );
#endif
