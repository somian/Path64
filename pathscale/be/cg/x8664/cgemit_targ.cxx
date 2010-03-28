/*
 * Copyright (C) 2008 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 *  Copyright 2009 SiCortex, Inc.
 */

/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* ====================================================================
 *
 * Module: cgemit_targ.c
 * $Revision: 1.173 $
 * $Date: 05/11/30 16:49:43-08:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: be/cg/x8664/SCCS/s.cgemit_targ.cxx $
 *
 * Description:
 *
 * Target-specific cgemit code.
 *
 * ====================================================================
 * ====================================================================
 */


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "elf_stuff.h"
#ifdef KEY /* Mac port */
#include "dwarf_stuff.h"
#endif /* KEY Mac port */

#define	USE_STANDARD_TYPES 1
#include "defs.h"
#include "config_targ_opt.h"
#include "targ_const.h"
#include "targ_const_private.h"
#include "vstring.h"
#include "config_asm.h"
#include "em_elf.h"
#include "symtab.h"
#include "tn.h"
#include "cgemit.h"
#include "cgemit_targ.h"
#include "cgtarget.h"
#include "data_layout.h"
#include "bb.h"
#include "op.h"
#include "iface_scn.h"
#include "cg_flags.h"
#include "glob.h"
#include "sections.h"
#include "targ_isa_print.h"
#include "config_debug.h"
#include "cg.h"

// Holds name of function used to retrieve the Instruction Pointer.
extern const char * ip_calc_funcname;

static ST *current_pu = NULL;

static BOOL
Non_Default_Text_Section (ST *pu)
{
  if (!pu || !ST_base(pu))
    return FALSE;

  return ((ST_sclass(ST_base(pu)) == SCLASS_TEXT) && 
	  strcmp(ST_name(ST_base(pu)), ".text"));
}


void
CGEMIT_Targ_Initialize (ST *pu)
{
  current_pu = pu;
}


void
CGEMIT_Targ_Text_Initialize (ST *pu)
{
  if (Non_Default_Text_Section(pu))
    fprintf (Asm_File, "\t.begin\tliteral_prefix %s\n", ST_name(ST_base(pu)));
}


void
CGEMIT_Targ_Text_Finalize (ST *pu)
{
  if (Non_Default_Text_Section(pu))
    fprintf (Asm_File, "\t.end\tliteral_prefix\n");
}


BOOL
CGEMIT_Align_Section_Once (const char *scn_name)
{
  return strcmp(scn_name, ".literal") && strcmp(scn_name, ".text");
}

void
CGEMIT_Prn_File_Dir_In_Asm(USRCPOS usrcpos,
                        const char *pathname,
                        const char *filename)
{
#if defined(BUILD_OS_DARWIN)
  /* .file takes a single string arg in v1.38 as */
  fprintf (Asm_File, "\t%s\t\"%s/%s\"\n", AS_FILE, pathname, filename);
#else /* defined(BUILD_OS_DARWIN) */
  if (CG_emit_non_gas_syntax)
    fprintf (Asm_File, "\t%s\t%d\t\"%s/%s\"\n",
	     AS_FILE, USRCPOS_filenum(usrcpos)-1, pathname, filename);
  else fprintf (Asm_File, "\t%s\t%d\t\"%s/%s\"\n",
	   AS_FILE, USRCPOS_filenum(usrcpos), pathname, filename);
#endif /* defined(BUILD_OS_DARWIN) */
}

extern void
CGEMIT_Prn_Line_Dir_In_Asm (USRCPOS usrcpos)
{
  if( !CG_emit_asm_dwarf) { 
    fprintf (Asm_File, " # "); //turn the rest into comment
  }
#if defined(BUILD_OS_DARWIN)
  /* Darwin as provides .line, not .loc */
  fprintf (Asm_File, "\t.line\t%d\n", 
	   USRCPOS_linenum(usrcpos));
#else /* defined(BUILD_OS_DARWIN) */
  if (CG_emit_non_gas_syntax)
    fprintf (Asm_File, "\t.loc\t%d\t%d\t%d\n", 
	     USRCPOS_filenum(usrcpos)-1,
	     USRCPOS_linenum(usrcpos),
	     USRCPOS_column(usrcpos));
  else
    fprintf (Asm_File, "\t.loc\t%d\t%d\t%d\n", 
	     USRCPOS_filenum(usrcpos),
	     USRCPOS_linenum(usrcpos),
	     USRCPOS_column(usrcpos));    
#endif /* defined(BUILD_OS_DARWIN) */
  }


void
CGEMIT_Prn_Scn_In_Asm (ST *st, ST *cur_section)
{
  UINT32 tmp, power;
  // Bug 511
  // Do not emit section attributes for the __libc_ sections. Assumes that
  // user inline assembly will do the job. We will avoid duplicate entries.
  {
    char *name = ST_name(st);
    if (strstr(name, "__libc_") == name)
      return;
  }
  power = 0;
  for (tmp = STB_align(st); tmp > 1; tmp >>= 1) power++;
  CGEMIT_Prn_Scn_In_Asm(Asm_File,
			ST_name(st),
			Get_Section_Elf_Type(STB_section_idx(st)),
			Get_Section_Elf_Flags(STB_section_idx(st)),
			Get_Section_Elf_Entsize(STB_section_idx(st)),
			power,
			(cur_section != NULL) ? ST_name(cur_section) : NULL);
}

void
CGEMIT_Prn_Scn_In_Asm (FILE       *asm_file,
		       const char *scn_name,
		       Elf64_Word  scn_type,
		       Elf64_Word  scn_flags,
		       Elf64_Xword scn_entsize,
		       Elf64_Word  scn_align,
		       const char *cur_scn_name)
{
  if ((cur_scn_name != NULL) && !strcmp(cur_scn_name, ".literal"))
  {

    /* If we added a prefix to the literal section, then end the
       prefix region */
    if (Non_Default_Text_Section(current_pu))
      fprintf (asm_file, "\t.end\tliteral_prefix\n");
  }
#if defined(BUILD_OS_DARWIN)
  /* Darwin uses ".lcomm", not ".bss" */
  else if (0 == strcmp(scn_name, BSS_RAW_NAME)) {
    return;
  }
#endif /* defined(BUILD_OS_DARWIN) */
  
  /* Handle .text, .data, and .literal specially. */

  if (!strcmp(scn_name, ".data") || !strcmp(scn_name, ".text"))
  {
    fprintf (asm_file, "\n\t%s", scn_name);
  }
  else if (!strcmp(scn_name, ".literal"))
  {
    /* If we need to add a prefix to the literal section, then emit
       the correct cabbage for that to happen. */
    if (Non_Default_Text_Section(current_pu))
    {
      CGEMIT_Prn_Scn_In_Asm(ST_base(current_pu), NULL);
      fprintf (asm_file, "\t.begin\tliteral_prefix %s\n",
	       ST_name(ST_base(current_pu)));
    }
    else
    {
      fprintf (asm_file, "\n\t.text\n");
    }
    fprintf (asm_file, "\t.literal_position\n");
  }
  else
  {
    char scn_flags_string[5];
    char *p = &scn_flags_string[0];
    
#if defined(BUILD_OS_DARWIN)
    fprintf (asm_file, "\n\t%s %s", AS_SECTION, map_section_name(scn_name));
#else /* defined(BUILD_OS_DARWIN) */
    fprintf (asm_file, "\n\t%s %s", AS_SECTION, scn_name);
#endif /* defined(BUILD_OS_DARWIN) */
    if (CG_emit_non_gas_syntax && strcmp(scn_name, ".srdata") == 0) {
      static BOOL printed = FALSE;
      if (!printed) {
	fprintf(asm_file, ", %d, %#x, %" SCNd64 ", ", 
		scn_type, scn_flags, scn_entsize);
	int tmp1 = 1, tmp2 = scn_align;
	for (; tmp2 >= 1; tmp1 *= 2, tmp2 --);
	fprintf(asm_file, "%d", tmp1);
	printed = TRUE;
      }
    }
#if ! defined(BUILD_OS_DARWIN)
    /* Mach-O assembler section directives don't use this syntax */
    if (! CG_emit_non_gas_syntax) {
      if ((scn_flags & SHF_WRITE) &&
          !(scn_name && !strncmp(scn_name,".gnu.linkonce.r.",16)))
        *p++ = 'w';
      if (scn_flags & SHF_ALLOC) *p++ = 'a';
      if (scn_flags & SHF_EXECINSTR) *p++ = 'x';
#ifdef KEY
      if (scn_flags & SHF_TLS) *p++ = 'T';
#endif
      *p = '\0'; // null terminate the string.
      fprintf (asm_file, ", \"%s\"", scn_flags_string);
      if (scn_type == SHT_PROGBITS)
        fprintf (asm_file, ",@progbits");
      else if (scn_type == SHT_NOBITS)
        fprintf (asm_file, ",@nobits");
    }
#endif /* defined(BUILD_OS_DARWIN) */
#ifdef TARG_X8664
#if ! defined(BUILD_OS_DARWIN)
    /* Mach-O linker rejects non-local labels in debug sections, but we are
     * already generating a local label for each section elsewhere. */
    if (strcmp(scn_name, DEBUG_FRAME_SECTNAME) == 0) // bug 2463
      fprintf(asm_file, "\n.LCIE:");
#endif /* defined(BUILD_OS_DARWIN) */

    // Generate a label at the start of the .eh_frame CIE
    if (!strcmp (scn_name, EH_FRAME_SECTNAME)) // bug 2729
      fprintf (asm_file, "\n.LEHCIE:");
#endif
  }

  fprintf (asm_file, "\n");

  /* For most sections, we only emit the alignment the first time we
     see it (in cgemit.cxx:Init_Section), because .org is used to
     place/align all data items relative to the start of the
     section. But some we always align. */

  if (!CGEMIT_Align_Section_Once(scn_name))
    fprintf (asm_file, "\t%s\t%d\n", AS_ALIGN, scn_align);
#if defined(BUILD_OS_DARWIN)
  /* Darwin "as" doesn't automatically define the section name to be a symbol
   * whose value is the origin of the section, so we must do it explicitly */
  fprintf(asm_file, "%s:\n", scn_name);
#endif /* defined(BUILD_OS_DARWIN) */
}

void
CGEMIT_Change_Origin_In_Asm (ST *st, INT64 offset)
{
  /* Make sure these match what is used in eh_region.cxx (with "t"
     changed to "e" or "h"). */
#define EH_REGION_LINKONCE_PREFIX ".gnu.linkonce.e."
#define EH_DESC_LINKONCE_PREFIX ".gnu.linkonce.h."
    
  /* We don't want to emit .org for literal sections, since the .org
     for these gets reset for every pu; and because we don't need them
     here.

     We don't want to emit .org for exception region or descriptors
     since the section contains both .xt_except_table/.xt_except_desc
     and .gnu.linkonce.e.* / .gnu.linkonce.h.* sections. We don't need
     the .org for these because there are no alignment issues since
     all INITVs in the section are 4 bytes, and the section start is 4
     byte aligned. */

  if (strcmp(ST_name(st), ".literal") &&
      strcmp(ST_name(st), ".xt_except_table") &&
      strcmp(ST_name(st), ".xt_desc_table") &&
      strncmp(ST_name(st), EH_REGION_LINKONCE_PREFIX,
	      strlen(EH_REGION_LINKONCE_PREFIX)) &&
      strncmp(ST_name(st), EH_DESC_LINKONCE_PREFIX,
	      strlen(EH_DESC_LINKONCE_PREFIX)))
  {
    if (CG_emit_non_gas_syntax)
      fprintf (Asm_File, "\t%s 0x%" SCNd64 "\n", ".origin", offset);
    else fprintf (Asm_File, "\t%s 0x%" SCNd64 "\n", AS_ORIGIN, offset);
    fprintf ( Asm_File, "\t%s\t0\n", AS_ALIGN );
  }
}


// whether to use the base st for the reloc
extern BOOL
CGEMIT_Use_Base_ST_For_Reloc (INT reloc, ST *st)
{
	if (reloc == TN_RELOC_IA_LTOFF_FPTR) 
		// gas doesn't like addends
		return FALSE;
	// to handle function pointers.
	// example: see gcc.c-torture/execute/func-ptr-1.c
	else if (ST_sclass(st) == SCLASS_TEXT)
	        return FALSE;
#ifdef KEY
	else if (ST_is_thread_local(st))
	        return FALSE;

	// bugs 14894, 15214
	else if (ST_sclass(st) == SCLASS_FSTATIC)
	        return FALSE;

	// bug 14894
	else if (ST_sclass(st) == SCLASS_PSTATIC &&
	         ST_name(st) &&
	         strchr(ST_name(st), '.'))
	        return FALSE;

	// bug 15211, sicortex 9249
	else if (ST_class(st) == CLASS_CONST)
		return FALSE;
#endif
	else 
		return ST_is_export_local(st);
}

	  
extern INT
CGEMIT_Relocs_In_Asm (TN *t, ST *st, vstring *buf, INT64 *val)
{
	INT paren = 1;	// num parens
	// only add in GP_DISP if based on gprel section
	// not if based on ipa-generated extern.
	if (ST_class(st) == CLASS_BLOCK && STB_section(st)) {
		*val -= GP_DISP;
	}
	switch (TN_relocs(t)) {
	case TN_RELOC_GOT_DISP:
        	*buf = vstr_concat (*buf, "%got_disp");
		break;
	case TN_RELOC_GOT_PAGE:
        	*buf = vstr_concat (*buf, "%got_page");
		break;
	case TN_RELOC_GOT_OFST:
        	*buf = vstr_concat (*buf, "%got_ofst");
		break;
	case TN_RELOC_HI_GPSUB:
        	*buf = vstr_concat (*buf, "%hi(%neg(%gp_rel");
		paren += 2;
		break;
	case TN_RELOC_LO_GPSUB:
        	*buf = vstr_concat (*buf, "%lo(%neg(%gp_rel");
		paren += 2;
		break;
	case TN_RELOC_GPREL16:
        	*buf = vstr_concat (*buf, "%gp_rel");
		break;
	case TN_RELOC_HIGH16:
        	*buf = vstr_concat (*buf, "%hi");
		break;
	case TN_RELOC_LOW16:
        	*buf = vstr_concat (*buf, "%lo");
		break;
	case TN_RELOC_X8664_64:
	  break;
	case TN_RELOC_X8664_PC32:
	  *buf = vstr_concat (*buf, "$");
	  break;
	case TN_RELOC_IA32_GOT:
	  if (ST_sym_class(st) == CLASS_CONST) {
	    char name[32];
	    sprintf (name, TCON_Label_Format, ST_IDX_index(ST_st_idx(st)));
	    *buf = vstr_concat (*buf, name);
	  }
	  else
	    *buf = vstr_concat (*buf, ST_name(st));
	  *buf = vstr_concat (*buf, "@GOT");
	  return 0;
	case TN_RELOC_X8664_GOTPCREL:
	  *buf = vstr_concat (*buf, ST_name(st));
	  *buf = vstr_concat (*buf, "@GOTPCREL");
	  return 0;
	case TN_RELOC_IA32_GLOBAL_OFFSET_TABLE:
	  {
	    char* str = NULL;
	    if (Is_Target_EM64T()    ||
                Is_Target_Wolfdale() ||
		Is_Target_Core())
	      asprintf( &str, "$_GLOBAL_OFFSET_TABLE_+[.-%s]", ST_name(st) );
	    else
	      asprintf( &str, "$_GLOBAL_OFFSET_TABLE_" );
	    *buf = vstr_concat( *buf, str );
	    free( str );
	  }
	  return 0;
	case TN_RELOC_X8664_TPOFF32_seg_reg:
	  *buf = vstr_concat(*buf, Is_Target_32bit() ? "%gs:" : "%fs:");
	  // fall through
	case TN_RELOC_X8664_TPOFF32:
	  *buf = vstr_concat(*buf, ST_name(st));
	  *buf = vstr_concat(*buf, Is_Target_32bit() ? "@NTPOFF" : "@TPOFF");
	  return 0;
	case TN_RELOC_X8664_GOTTPOFF:
	  *buf = vstr_concat(*buf, ST_name(st));
	  *buf = vstr_concat(*buf,
			     Is_Target_32bit() ? "@INDNTPOFF" : "@GOTTPOFF");
	  return 0;
    	default:
		#pragma mips_frequency_hint NEVER
    		FmtAssert (FALSE, ("relocs_asm: illegal reloc TN"));
		/*NOTREACHED*/
	}
	*buf = vstr_concat (*buf, "(" );
	if (ST_sym_class(st) == CLASS_CONST) {
	  char name[32];
	  sprintf (name, TCON_Label_Format, ST_IDX_index(ST_st_idx(st)));
	  *buf = vstr_concat (*buf, name);
	}
	else
	*buf = vstr_concat (*buf, ST_name(st));
	*buf = vstr_concat (*buf, Symbol_Name_Suffix);
	return paren;
}


extern void
CGEMIT_Relocs_In_Object (TN *t, ST *st, INT32 PC, pSCNINFO PU_section, INT64 *val)
{
  FmtAssert(FALSE, ("NYI"));
} 

// add events and relocs as needed for call
extern void 
CGEMIT_Add_Call_Information (OP *op, BB *bb, INT32 PC, pSCNINFO PU_section)
{
	ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_CALLINFO);
	ST *call_sym = CALLINFO_call_st(ANNOT_callinfo(ant));
    	Elf_Event_Kind event_type;

	if (call_sym == NULL) return;
	if (ST_is_export_local(call_sym)) {
		event_type = EK_FCALL_LOCAL;
	}
	else {
		event_type = EK_FCALL_EXTERN;
      	}
	Em_Add_New_Event (event_type, PC, EMT_Put_Elf_Symbol(call_sym),
			0, 0, PU_section);
      
	// TODO: if indirect call add plt reloc

	// do pcrel relocation for all calls,
	// as even statics may be forward refs so don't know pc.
	// Ld will generate a stub if needed.
	Em_Add_New_Rela (EMT_Put_Elf_Symbol(call_sym), 
		R_IA_64_PCREL21B, PC, 0, PU_section);

      	if (EMIT_interface_section) {
		Interface_Scn_Add_Call( call_sym, 
			CALLINFO_call_wn(ANNOT_callinfo(ant)));
      	}
}


/* Generate the .frame, .mask and the .fmask directives for the assembler. */
void
CGEMIT_Gen_Asm_Frame (INT64 frame_len)
{
  if (CG_inhibit_size_directive)
    return;
  TN *tn = ((Current_PU_Stack_Model == SMODEL_SMALL) ? SP_TN : FP_TN);
  ISA_REGISTER_CLASS rc = TN_register_class(tn);
  REGISTER reg = TN_register(tn);
  fprintf ( Asm_File, "\t%s\t%s, %" SCNd64 ", %s\n",
	    AS_FRAME,
	    ABI_PROPERTY_Reg_Name(rc, REGISTER_machine_id(rc, reg)),
	    frame_len,
	    ABI_PROPERTY_Reg_Name(rc, REGISTER_machine_id(rc, reg)));
}


// Generate the entry (.proc) directive.
void 
CGEMIT_Prn_Ent_In_Asm (ST *pu)
{
  FmtAssert(false, ("No AS_ENT for x86_64"));
}


// Preprocess FP registers before emit.  Needed only for IA-32.
void
STACK_FP_Fixup_PU()
{}

void
CGEMIT_Weak_Alias (ST *sym, ST *strongsym) 
{
  fprintf ( Asm_File, "\t%s\t%s\n", AS_WEAK, ST_name(sym));
  CGEMIT_Alias (sym, strongsym);
}

void CGEMIT_Write_Literal_TCON(ST *lit_st, TCON tcon)
{
  INT64 val;
  if (TCON_ty(tcon) == MTYPE_F4)
    val = TCON_word0(tcon);
  else if ((TCON_ty(tcon) == MTYPE_I4) || (TCON_ty(tcon) == MTYPE_U4))
    val = TCON_v0(tcon);
  else
    FmtAssert(FALSE, ("Invalid literal value"));
  fprintf ( Asm_File, "\t%s\t", ".literal");
  EMT_Write_Qualified_Name(Asm_File, lit_st);
  if ((val >= INT32_MIN) && (val <= INT32_MAX)) 
    fprintf(Asm_File, ", %" SCNd64 "\n", val);
  else
    fprintf(Asm_File, ", %#" SCNx64 "\n", val);
  
}

void CGEMIT_Write_Literal_Label (ST *lit_st, LABEL_IDX lab)
{
  fprintf ( Asm_File, "\t%s\t", ".literal");
  EMT_Write_Qualified_Name(Asm_File, lit_st);
  union {
    UINT64 u;
    void *p;
  } u;
  u.u = 0;
  u.p = LABEL_name(lab);
  fprintf(Asm_File, ", %" SCNd64 "\n", u.u);
}

void CGEMIT_Write_Literal_Symbol (ST *lit_st, ST *sym, 
				  Elf64_Sxword sym_ofst)
{
  ST *basesym;
  basesym = sym;
  INT64 base_ofst = 0;

  if (Has_Base_Block(sym) && ST_is_export_local(sym) && ST_class(sym) != CLASS_FUNC) {
    Base_Symbol_And_Offset (sym, &basesym, &base_ofst);
  }
  base_ofst += sym_ofst;

  fprintf ( Asm_File, "\t%s\t", ".literal");
  EMT_Write_Qualified_Name(Asm_File, lit_st);
  fprintf ( Asm_File, ", ");
  if (ST_class(sym) == CLASS_CONST) {
    EMT_Write_Qualified_Name (Asm_File, basesym);
    if (base_ofst == 0)
      fprintf (Asm_File, "\n");
    else
      fprintf (Asm_File, " %+" SCNd64 "\n", base_ofst);
  }
  else {
    EMT_Write_Qualified_Name (Asm_File, sym);
    if (sym_ofst == 0)
      fprintf (Asm_File, "\n");
    else
      fprintf (Asm_File, " %+" SCNd64 "\n", sym_ofst);
  }
}

void CGEMIT_Alias (ST *sym, ST *strongsym) 
{
  // bug 14491: alias statement should write out the qualified name
  fprintf ( Asm_File, "\t%s = %s", ST_name(sym), ST_name(strongsym));
  if (ST_is_export_local(strongsym) && ST_class(strongsym) == CLASS_VAR) {
    // modelled after EMT_Write_Qualified_Name (bug 6899)
    if (ST_level(strongsym) == GLOBAL_SYMTAB) {
      // bug 14517
      if (Emit_Global_Data || (ST_sclass(strongsym) == SCLASS_PSTATIC &&
                               !strchr (ST_name(strongsym), '.')))
        fprintf ( Asm_File, "%s%d", Label_Name_Separator, ST_index(strongsym));
    }
    else if (!PU_c_lang(Get_Current_PU()) || !strchr (ST_name(strongsym), '.'))
      fprintf ( Asm_File, "%s%d%s%d", Label_Name_Separator, 
		ST_pu(Get_Current_PU_ST()),
      		Label_Name_Separator, ST_index(strongsym));
  }
  fprintf ( Asm_File, "\n");
}


////////////////////////////////////////////////////////////////////////////////////////
//
//    Emit the instructions to the .s file.
//
////////////////////////////////////////////////////////////////////////////////////////

static const char* OP_Name[TOP_count];

static void Init_OP_Name()
{
  static BOOL bInit = FALSE;

  if( bInit )
    return;

  bInit = TRUE;
  bzero( OP_Name, sizeof(OP_Name) );

  // Only put in the name which is different from isa.cxx.

  OP_Name[TOP_reti]  = "ret";
  OP_Name[TOP_comixsd]  = "comisd";
  OP_Name[TOP_comixxsd] = "comisd";
  OP_Name[TOP_comixxxsd]= "comisd";
  OP_Name[TOP_comixss]  = "comiss";
  OP_Name[TOP_comixxss] = "comiss";
  OP_Name[TOP_comixxxss]= "comiss";
  OP_Name[TOP_add32] = "addl";
  OP_Name[TOP_adc32] = "adcl";
  OP_Name[TOP_add64] = "addq";
  OP_Name[TOP_addx32] = "addl";
  OP_Name[TOP_addx64] = "addq";
  OP_Name[TOP_addxx32]  = "addl";
  OP_Name[TOP_addxxx32] = "addl";
  OP_Name[TOP_addxx64]  = "addq";
  OP_Name[TOP_addxxx64] = "addq";
  OP_Name[TOP_addxss] = "addss";
  OP_Name[TOP_addxsd] = "addsd";
  OP_Name[TOP_addxxss]  = "addss";
  OP_Name[TOP_addxxxss] = "addss";
  OP_Name[TOP_addxxsd]  = "addsd";
  OP_Name[TOP_addxxxsd] = "addsd";
  OP_Name[TOP_add128v8] = "paddb";
  OP_Name[TOP_addx128v8] = "paddb";
  OP_Name[TOP_addxx128v8] = "paddb";
  OP_Name[TOP_addxxx128v8] = "paddb";
  OP_Name[TOP_mul128v16] = "pmullw";
  OP_Name[TOP_add128v16] = "paddw";
  OP_Name[TOP_addx128v16] = "paddw";
  OP_Name[TOP_addxx128v16] = "paddw";
  OP_Name[TOP_addxxx128v16] = "paddw";
  OP_Name[TOP_add128v32] = "paddd";
  OP_Name[TOP_addx128v32] = "paddd";
  OP_Name[TOP_addxx128v32] = "paddd";
  OP_Name[TOP_addxxx128v32] = "paddd";
  OP_Name[TOP_add128v64] = "paddq";
  OP_Name[TOP_addx128v64] = "paddq";
  OP_Name[TOP_addxx128v64] = "paddq";
  OP_Name[TOP_addxxx128v64] = "paddq";
  OP_Name[TOP_fadd128v32] = "addps";
  OP_Name[TOP_faddx128v32] = "addps";
  OP_Name[TOP_faddxx128v32] = "addps";
  OP_Name[TOP_faddxxx128v32] = "addps";
  OP_Name[TOP_fadd128v64] = "addpd";
  OP_Name[TOP_faddx128v64] = "addpd";
  OP_Name[TOP_faddxx128v64] = "addpd";
  OP_Name[TOP_faddxxx128v64] = "addpd";
  OP_Name[TOP_faddsub128v32] = "addsubps";
  OP_Name[TOP_faddsubx128v32] = "addsubps";
  OP_Name[TOP_faddsubxx128v32] = "addsubps";
  OP_Name[TOP_faddsubxxx128v32] = "addsubps";
  OP_Name[TOP_faddsub128v64] = "addsubpd";
  OP_Name[TOP_faddsubx128v64] = "addsubpd";
  OP_Name[TOP_faddsubxx128v64] = "addsubpd";
  OP_Name[TOP_faddsubxxx128v64] = "addsubpd";
  OP_Name[TOP_fhadd128v32] = "haddps";
  OP_Name[TOP_fhaddx128v32] = "haddps";
  OP_Name[TOP_fhaddxx128v32] = "haddps";
  OP_Name[TOP_fhaddxxx128v32] = "haddps";
  OP_Name[TOP_fhadd128v64] = "haddpd";
  OP_Name[TOP_fhaddx128v64] = "haddpd";
  OP_Name[TOP_fhaddxx128v64] = "haddpd";
  OP_Name[TOP_fhaddxxx128v64] = "haddpd";
  OP_Name[TOP_fsub128v32] = "hsubps";
  OP_Name[TOP_fsubx128v32] = "hsubps";
  OP_Name[TOP_fsubxx128v32] = "hsubps";
  OP_Name[TOP_fsubxxx128v32] = "hsubps";
  OP_Name[TOP_fsub128v64] = "hsubpd";
  OP_Name[TOP_fsubx128v64] = "hsubpd";
  OP_Name[TOP_fsubxx128v64] = "hsubpd";
  OP_Name[TOP_fsubxxx128v64] = "hsubpd";
  OP_Name[TOP_fmovsldup] = "movsldup";
  OP_Name[TOP_fmovshdup] = "movshdup";
  OP_Name[TOP_fmovddup] = "movddup";
  OP_Name[TOP_fmovsldupx] = "movsldup";
  OP_Name[TOP_fmovshdupx] = "movshdup";
  OP_Name[TOP_fmovddupx] = "movddup";
  OP_Name[TOP_fmovsldupxx] = "movsldup";
  OP_Name[TOP_fmovshdupxx] = "movshdup";
  OP_Name[TOP_fmovddupxx] = "movddup";
  OP_Name[TOP_fmovsldupxxx] = "movsldup";
  OP_Name[TOP_fmovshdupxxx] = "movshdup";
  OP_Name[TOP_fmovddupxxx] = "movddup";
  OP_Name[TOP_addi32] = "addl";
  OP_Name[TOP_adci32] = "adcl";
  OP_Name[TOP_addi64] = "addq";
  OP_Name[TOP_imuli32] = "imull";
  OP_Name[TOP_imuli64] = "imulq";
  OP_Name[TOP_and32] = "andl";
  OP_Name[TOP_andx32]  = "andl";
  OP_Name[TOP_andxx32] = "andl";
  OP_Name[TOP_andxxx32]= "andl";
  OP_Name[TOP_and64]   = "andq";
  OP_Name[TOP_andx64]  = "andq";
  OP_Name[TOP_andxx64] = "andq";
  OP_Name[TOP_andxxx64]= "andq";
  OP_Name[TOP_andi32] = "andl";
  OP_Name[TOP_andi64] = "andq";
  OP_Name[TOP_and128v8] = "pand";
  OP_Name[TOP_andx128v8] = "pand";
  OP_Name[TOP_andxx128v8] = "pand";
  OP_Name[TOP_andxxx128v8] = "pand";
  OP_Name[TOP_and128v16] = "pand";
  OP_Name[TOP_andx128v16] = "pand";
  OP_Name[TOP_andxx128v16] = "pand";
  OP_Name[TOP_andxxx128v16] = "pand";
  OP_Name[TOP_and128v32] = "pand";
  OP_Name[TOP_andx128v32] = "pand";
  OP_Name[TOP_andxx128v32] = "pand";
  OP_Name[TOP_andxxx128v32] = "pand";
  OP_Name[TOP_and128v64] = "pand";
  OP_Name[TOP_andx128v64] = "pand";
  OP_Name[TOP_andxx128v64] = "pand";
  OP_Name[TOP_andxxx128v64] = "pand";
  OP_Name[TOP_andn128v8] = "pandn";
  OP_Name[TOP_andnx128v8] = "pandn";
  OP_Name[TOP_andnxx128v8] = "pandn";
  OP_Name[TOP_andnxxx128v8] = "pandn";
  OP_Name[TOP_andn128v16] = "pandn";
  OP_Name[TOP_andnx128v16] = "pandn";
  OP_Name[TOP_andnxx128v16] = "pandn";
  OP_Name[TOP_andnxxx128v16] = "pandn";
  OP_Name[TOP_andn128v32] = "pandn";
  OP_Name[TOP_andnx128v32] = "pandn";
  OP_Name[TOP_andnxx128v32] = "pandn";
  OP_Name[TOP_andnxxx128v32] = "pandn";
  OP_Name[TOP_andn128v64] = "pandn";
  OP_Name[TOP_andnx128v64] = "pandn";
  OP_Name[TOP_andnxx128v64] = "pandn";
  OP_Name[TOP_andnxxx128v64] = "pandn";
  OP_Name[TOP_fand128v32] = "andps";
  OP_Name[TOP_fandx128v32] = "andps";
  OP_Name[TOP_fandxx128v32] = "andps";
  OP_Name[TOP_fandxxx128v32] = "andps";
  OP_Name[TOP_fand128v64] = "andpd";
  OP_Name[TOP_fandx128v64] = "andpd";
  OP_Name[TOP_fandxx128v64] = "andpd";
  OP_Name[TOP_fandxxx128v64] = "andpd";
  OP_Name[TOP_or128v8] = "por";
  OP_Name[TOP_orx128v8] = "por";
  OP_Name[TOP_orxx128v8] = "por";
  OP_Name[TOP_orxxx128v8] = "por";
  OP_Name[TOP_or128v16] = "por";
  OP_Name[TOP_orx128v16] = "por";
  OP_Name[TOP_orxx128v16] = "por";
  OP_Name[TOP_orxxx128v16] = "por";
  OP_Name[TOP_or128v32] = "por";
  OP_Name[TOP_orx128v32] = "por";
  OP_Name[TOP_orxx128v32] = "por";
  OP_Name[TOP_orxxx128v32] = "por";
  OP_Name[TOP_or128v64] = "por";
  OP_Name[TOP_orx128v64] = "por";
  OP_Name[TOP_orxx128v64] = "por";
  OP_Name[TOP_orxxx128v64] = "por";
  OP_Name[TOP_for128v32] = "orps";
  OP_Name[TOP_forx128v32] = "orps";
  OP_Name[TOP_forxx128v32] = "orps";
  OP_Name[TOP_forxxx128v32] = "orps";
  OP_Name[TOP_for128v64] = "orpd";
  OP_Name[TOP_forx128v64] = "orpd";
  OP_Name[TOP_forxx128v64] = "orpd";
  OP_Name[TOP_forxxx128v64] = "orpd";
  OP_Name[TOP_xor128v8] = "pxor";
  OP_Name[TOP_xorx128v8] = "pxor";
  OP_Name[TOP_xorxx128v8] = "pxor";
  OP_Name[TOP_xorxxx128v8] = "pxor";
  OP_Name[TOP_xor128v16] = "pxor";
  OP_Name[TOP_xorx128v16] = "pxor";
  OP_Name[TOP_xorxx128v16] = "pxor";
  OP_Name[TOP_xorxxx128v16] = "pxor";
  OP_Name[TOP_xor128v32] = "pxor";
  OP_Name[TOP_xorx128v32] = "pxor";
  OP_Name[TOP_xorxx128v32] = "pxor";
  OP_Name[TOP_xorxxx128v32] = "pxor";
  OP_Name[TOP_xor128v64] = "pxor";
  OP_Name[TOP_xorx128v64] = "pxor";
  OP_Name[TOP_xorxx128v64] = "pxor";
  OP_Name[TOP_xorxxx128v64] = "pxor";
  OP_Name[TOP_fxor128v32] = "xorps";
  OP_Name[TOP_fxorx128v32] = "xorps";
  OP_Name[TOP_fxorxx128v32] = "xorps";
  OP_Name[TOP_fxorxxx128v32] = "xorps";
  OP_Name[TOP_fxor128v64] = "xorpd";
  OP_Name[TOP_fxorx128v64] = "xorpd";
  OP_Name[TOP_fxorxx128v64] = "xorpd";
  OP_Name[TOP_fxorxxx128v64] = "xorpd";
  OP_Name[TOP_fmax128v32] = "maxps";
  OP_Name[TOP_fmaxx128v32] = "maxps";
  OP_Name[TOP_fmaxxx128v32] = "maxps";
  OP_Name[TOP_fmaxxxx128v32] = "maxps";
  OP_Name[TOP_fmax128v64] = "maxpd";
  OP_Name[TOP_fmaxx128v64] = "maxpd";
  OP_Name[TOP_fmaxxx128v64] = "maxpd";
  OP_Name[TOP_fmaxxxx128v64] = "maxpd";
  OP_Name[TOP_fmin128v32] = "minps";
  OP_Name[TOP_fminx128v32] = "minps";
  OP_Name[TOP_fminxx128v32] = "minps";
  OP_Name[TOP_fminxxx128v32] = "minps";
  OP_Name[TOP_fmin128v64] = "minpd";
  OP_Name[TOP_fminx128v64] = "minpd";
  OP_Name[TOP_fminxx128v64] = "minpd";
  OP_Name[TOP_fminxxx128v64] = "minpd";
  OP_Name[TOP_fdiv128v32] = "divps";
  OP_Name[TOP_fdivx128v32] = "divps";
  OP_Name[TOP_fdivxx128v32] = "divps";
  OP_Name[TOP_fdivxxx128v32] = "divps";
  OP_Name[TOP_fdiv128v64] = "divpd";
  OP_Name[TOP_fdivx128v64] = "divpd";
  OP_Name[TOP_fdivxx128v64] = "divpd";
  OP_Name[TOP_fdivxxx128v64] = "divpd";
  OP_Name[TOP_fmul128v32] = "mulps";
  OP_Name[TOP_fmulx128v32] = "mulps";
  OP_Name[TOP_fmulxx128v32] = "mulps";
  OP_Name[TOP_fmulxxx128v32] = "mulps";
  OP_Name[TOP_fmul128v64] = "mulpd";
  OP_Name[TOP_fmulx128v64] = "mulpd";
  OP_Name[TOP_fmulxx128v64] = "mulpd";
  OP_Name[TOP_fmulxxx128v64] = "mulpd";
  OP_Name[TOP_frsqrt128v32] = "rsqrtps";
  OP_Name[TOP_fsqrt128v32] = "sqrtps";
  OP_Name[TOP_fsqrt128v64] = "sqrtpd";
  OP_Name[TOP_frcp128v32] = "rcpps";
  OP_Name[TOP_subus128v16] = "psubusw";
  OP_Name[TOP_cmpgt128v8] = "pcmpgtb";
  OP_Name[TOP_cmpgt128v16] = "pcmpgtw";
  OP_Name[TOP_cmpgt128v32] = "pcmpgtd";
  OP_Name[TOP_cmpeq128v8] = "pcmpeqb";
  OP_Name[TOP_cmpeq128v16] = "pcmpeqw";
  OP_Name[TOP_cmpeq128v32] = "pcmpeqd";
  OP_Name[TOP_cmpgtx128v8] = "pcmpgtb";
  OP_Name[TOP_cmpgtx128v16] = "pcmpgtw";
  OP_Name[TOP_cmpgtx128v32] = "pcmpgtd";
  OP_Name[TOP_cmpeqx128v8] = "pcmpeqb";
  OP_Name[TOP_cmpeqx128v16] = "pcmpeqw";
  OP_Name[TOP_cmpeqx128v32] = "pcmpeqd";
  OP_Name[TOP_cmpgtxx128v8] = "pcmpgtb";
  OP_Name[TOP_cmpgtxx128v16] = "pcmpgtw";
  OP_Name[TOP_cmpgtxx128v32] = "pcmpgtd";
  OP_Name[TOP_cmpeqxx128v8] = "pcmpeqb";
  OP_Name[TOP_cmpeqxx128v16] = "pcmpeqw";
  OP_Name[TOP_cmpeqxx128v32] = "pcmpeqd";
  OP_Name[TOP_cmpgtxxx128v8] = "pcmpgtb";
  OP_Name[TOP_cmpgtxxx128v16] = "pcmpgtw";
  OP_Name[TOP_cmpgtxxx128v32] = "pcmpgtd";
  OP_Name[TOP_cmpeqxxx128v8] = "pcmpeqb";
  OP_Name[TOP_cmpeqxxx128v16] = "pcmpeqw";
  OP_Name[TOP_cmpeqxxx128v32] = "pcmpeqd";
  OP_Name[TOP_max128v8] = "pmaxub";
  OP_Name[TOP_max128v16] = "pmaxsw";
  OP_Name[TOP_maxx128v8] = "pmaxub";
  OP_Name[TOP_maxx128v16] = "pmaxsw";
  OP_Name[TOP_maxxx128v8] = "pmaxub";
  OP_Name[TOP_maxxx128v16] = "pmaxsw";
  OP_Name[TOP_maxxxx128v8] = "pmaxub";
  OP_Name[TOP_maxxxx128v16] = "pmaxsw";
  OP_Name[TOP_min128v8] = "pmaxub";
  OP_Name[TOP_min128v16] = "pmaxsw";
  OP_Name[TOP_minx128v8] = "pmaxub";
  OP_Name[TOP_minx128v16] = "pmaxsw";
  OP_Name[TOP_minxx128v8] = "pmaxub";
  OP_Name[TOP_minxx128v16] = "pmaxsw";
  OP_Name[TOP_minxxx128v8] = "pmaxub";
  OP_Name[TOP_minxxx128v16] = "pmaxsw";
  OP_Name[TOP_icall]    = "call";
  OP_Name[TOP_icallx]   = "call";
  OP_Name[TOP_icallxx]  = "call";
  OP_Name[TOP_icallxxx] = "call";
  OP_Name[TOP_ijmp]    = "jmp";
  OP_Name[TOP_ijmpx]   = "jmp";
  OP_Name[TOP_ijmpxx]  = "jmp";
  OP_Name[TOP_ijmpxxx] = "jmp";
  OP_Name[TOP_xor8]   = "xorb";
  OP_Name[TOP_xorx8]  = "xorb";
  OP_Name[TOP_xorxx8] = "xorb";
  OP_Name[TOP_xorxxx8]= "xorb";
  OP_Name[TOP_xor16]   = "xorw";
  OP_Name[TOP_xorx16]  = "xorw";
  OP_Name[TOP_xorxx16] = "xorw";
  OP_Name[TOP_xorxxx16]= "xorw";
  OP_Name[TOP_and8]   = "andb";
  OP_Name[TOP_andx8]  = "andb";
  OP_Name[TOP_andxx8] = "andb";
  OP_Name[TOP_andxxx8]= "andb";
  OP_Name[TOP_and16]   = "andw";
  OP_Name[TOP_andx16]  = "andw";
  OP_Name[TOP_andxx16] = "andw";
  OP_Name[TOP_andxxx16]= "andw";
  OP_Name[TOP_or8]   = "orb";
  OP_Name[TOP_orx8]  = "orb";
  OP_Name[TOP_orxx8] = "orb";
  OP_Name[TOP_orxxx8]= "orb";
  OP_Name[TOP_or16]   = "orw";
  OP_Name[TOP_orx16]  = "orw";
  OP_Name[TOP_orxx16] = "orw";
  OP_Name[TOP_orxxx16]= "orw";
  OP_Name[TOP_cmp8]   = "cmpb";
  OP_Name[TOP_cmpx8]  = "cmpb";
  OP_Name[TOP_cmpxx8] = "cmpb";
  OP_Name[TOP_cmpxxx8]= "cmpb";
  OP_Name[TOP_cmpi8]   = "cmpb";
  OP_Name[TOP_cmpxi8]  = "cmpb";
  OP_Name[TOP_cmpxxi8] = "cmpb";
  OP_Name[TOP_cmpxxxi8]= "cmpb";
  OP_Name[TOP_cmpxr8]  = "cmpb";
  OP_Name[TOP_cmpxxr8] = "cmpb";
  OP_Name[TOP_cmpxxxr8]= "cmpb";
  OP_Name[TOP_cmp16]   = "cmpw";
  OP_Name[TOP_cmpx16]  = "cmpw";
  OP_Name[TOP_cmpxx16] = "cmpw";
  OP_Name[TOP_cmpxxx16]= "cmpw";
  OP_Name[TOP_cmpi16]   = "cmpw";
  OP_Name[TOP_cmpxi16]  = "cmpw";
  OP_Name[TOP_cmpxxi16] = "cmpw";
  OP_Name[TOP_cmpxxxi16]= "cmpw";
  OP_Name[TOP_cmpxr16]  = "cmpw";
  OP_Name[TOP_cmpxxr16] = "cmpw";
  OP_Name[TOP_cmpxxxr16]= "cmpw";
  OP_Name[TOP_cmp32]   = "cmpl";
  OP_Name[TOP_cmpx32]  = "cmpl";
  OP_Name[TOP_cmpxx32] = "cmpl";
  OP_Name[TOP_cmpxxx32]= "cmpl";
  OP_Name[TOP_cmpxi32]  = "cmpl";
  OP_Name[TOP_cmpxxi32] = "cmpl";
  OP_Name[TOP_cmpxxxi32]= "cmpl";
  OP_Name[TOP_cmpxr32]  = "cmpl";
  OP_Name[TOP_cmpxxr32] = "cmpl";
  OP_Name[TOP_cmpxxxr32]= "cmpl";
  OP_Name[TOP_cmp64]   = "cmpq";
  OP_Name[TOP_cmpx64]  = "cmpq";
  OP_Name[TOP_cmpxx64] = "cmpq";
  OP_Name[TOP_cmpxxx64]= "cmpq";
  OP_Name[TOP_cmpxi64]  = "cmpq";
  OP_Name[TOP_cmpxxi64] = "cmpq";
  OP_Name[TOP_cmpxxxi64]= "cmpq";
  OP_Name[TOP_cmpxr64]  = "cmpq";
  OP_Name[TOP_cmpxxr64] = "cmpq";
  OP_Name[TOP_cmpxxxr64]= "cmpq";
  OP_Name[TOP_cmpi32]  = "cmpl";
  OP_Name[TOP_cmpi64]  = "cmpq";
  //OP_Name[TOP_dec32] = "decl";
  //OP_Name[TOP_dec64] = "decq";
  OP_Name[TOP_div32] = "divl";
  OP_Name[TOP_div64] = "divq";
  OP_Name[TOP_idiv32] = "idivl";
  OP_Name[TOP_idiv64] = "idivq";
  //OP_Name[TOP_inc32] = "incl";
  //OP_Name[TOP_inc64] = "incq";
  OP_Name[TOP_lea32]   = "leal";
  OP_Name[TOP_lea64]   = "leaq";
  OP_Name[TOP_leax32]  = "leal";
  OP_Name[TOP_leax64]  = "leaq";
  OP_Name[TOP_leaxx32] = "leal";
  OP_Name[TOP_leaxx64] = "leaq";
  OP_Name[TOP_prefetchx]   = "prefetch";
  OP_Name[TOP_prefetchxx]  = "prefetch";
  OP_Name[TOP_prefetchwx]  = "prefetchw";
  OP_Name[TOP_prefetchwxx] = "prefetchw";
  OP_Name[TOP_prefetcht0x] = "prefetcht0";
  OP_Name[TOP_prefetcht0xx]= "prefetcht0";
  OP_Name[TOP_prefetcht1x] = "prefetcht1";
  OP_Name[TOP_prefetcht1xx]= "prefetcht1";
  OP_Name[TOP_prefetchntax] = "prefetchnta";
  OP_Name[TOP_prefetchntaxx]= "prefetchnta";
  if (CG_use_prefetchnta) {
    OP_Name[TOP_prefetcht0] = "prefetchnta";
    OP_Name[TOP_prefetcht0x] = "prefetchnta";
    OP_Name[TOP_prefetcht0xx]= "prefetchnta";
  }
  OP_Name[TOP_ld8_32]   = "movsbl";
  OP_Name[TOP_ldx8_32]  = "movsbl";
  OP_Name[TOP_ldxx8_32] = "movsbl";
  OP_Name[TOP_ldu8_32]  = "movzbl";
  OP_Name[TOP_ldxu8_32] = "movzbl";
  OP_Name[TOP_ldxxu8_32]= "movzbl";
  OP_Name[TOP_ld8_32_n32]   = "movsbl";
  OP_Name[TOP_ldu8_32_n32]  = "movzbl";

  OP_Name[TOP_ld16_32]   = "movswl";
  OP_Name[TOP_ldx16_32]  = "movswl";
  OP_Name[TOP_ldxx16_32] = "movswl";
  OP_Name[TOP_ldu16_32]  = "movzwl";
  OP_Name[TOP_ldxu16_32] = "movzwl";
  OP_Name[TOP_ldxxu16_32]= "movzwl";
  OP_Name[TOP_ld16_32_n32]   = "movswl";
  OP_Name[TOP_ldu16_32_n32]  = "movzwl";

  OP_Name[TOP_ld8_64]		= "movsbq";
  OP_Name[TOP_ldx8_64]		= "movsbq";
  OP_Name[TOP_ldxx8_64]		= "movsbq";
  OP_Name[TOP_ld8_64_off]	= "movsbq";
  OP_Name[TOP_ldu8_64]		= "movzbq";
  OP_Name[TOP_ldxu8_64]		= "movzbq";
  OP_Name[TOP_ldxxu8_64]	= "movzbq";
  OP_Name[TOP_ldu8_64_off]	= "movzbq";
  OP_Name[TOP_ld16_64]		= "movswq";
  OP_Name[TOP_ldx16_64]		= "movswq";
  OP_Name[TOP_ldxx16_64]	= "movswq";
  OP_Name[TOP_ld16_64_off]	= "movswq";
  OP_Name[TOP_ldu16_64]		= "movzwq";
  OP_Name[TOP_ldxu16_64]	= "movzwq";
  OP_Name[TOP_ldxxu16_64]	= "movzwq";
  OP_Name[TOP_ldu16_64_off]	= "movzwq";
  OP_Name[TOP_ld32_64_off]	= "movslq";

  OP_Name[TOP_ld32]  = "movl";
  OP_Name[TOP_ldx32] = "movl";
  OP_Name[TOP_ldxx32]= "movl";
  OP_Name[TOP_ld32_n32]  = "movl";
  OP_Name[TOP_ld64]  = "movq";
  OP_Name[TOP_ld64_off]  = "movq";
  OP_Name[TOP_ldx64] = "movq";
  OP_Name[TOP_ldxx64]= "movq";
  OP_Name[TOP_zero32] = "xorl";
  OP_Name[TOP_zero64] = "xorq";
  OP_Name[TOP_xzero32] = "xorps";
  OP_Name[TOP_xzero64] = "xorpd";
  OP_Name[TOP_xzero128v32] = "xorps";
  OP_Name[TOP_xzero128v64] = "xorpd";
  OP_Name[TOP_inc8] = "incb";
  OP_Name[TOP_inc16] = "incw";
  OP_Name[TOP_inc32] = "incl";
  OP_Name[TOP_inc64] = "incq";
  OP_Name[TOP_dec8] = "decb";
  OP_Name[TOP_dec16] = "decw";
  OP_Name[TOP_dec32] = "decl";
  OP_Name[TOP_dec64] = "decq";
  OP_Name[TOP_ldc32] = "movl";
  OP_Name[TOP_ldc64] = "movq";
  OP_Name[TOP_ld32_64]   = "movslq";
  OP_Name[TOP_ldx32_64]  = "movslq";
  OP_Name[TOP_ldxx32_64] = "movslq";
  OP_Name[TOP_mov32] = "movl";
  OP_Name[TOP_mov64] = "movq";

  OP_Name[TOP_movzlq] = "movl";

  OP_Name[TOP_fstps_n32] = "fstps";
  OP_Name[TOP_fstpl_n32] = "fstpl";
  OP_Name[TOP_fstpt_n32] = "fstpt";
  OP_Name[TOP_fsts_n32] = "fsts";
  OP_Name[TOP_fstl_n32] = "fstl";

  OP_Name[TOP_flds_n32]  = "flds";
  OP_Name[TOP_fldl_n32]  = "fldl";
  OP_Name[TOP_fldt_n32]  = "fldt";

  OP_Name[TOP_divxsd]  = "divsd";
  OP_Name[TOP_divxxsd] = "divsd";
  OP_Name[TOP_divxxxsd]= "divsd";
  OP_Name[TOP_divxss]  = "divss";
  OP_Name[TOP_divxxss] = "divss";
  OP_Name[TOP_divxxxss]= "divss";
  OP_Name[TOP_imul32] = "imull";
  OP_Name[TOP_imul64] = "imulq";
  OP_Name[TOP_imulx64] = "imulq";
  OP_Name[TOP_imulx32] = "imull";
  OP_Name[TOP_mul32] = "mull";
  OP_Name[TOP_mulx64] = "mulq";
  OP_Name[TOP_mulxss] = "mulss";
  OP_Name[TOP_mulxsd] = "mulsd";
  OP_Name[TOP_mulxxss] = "mulss";
  OP_Name[TOP_mulxxxss]= "mulss";
  OP_Name[TOP_mulxxsd] = "mulsd";
  OP_Name[TOP_mulxxxsd]= "mulsd";
  OP_Name[TOP_fmul128v32] = "mulps";
  OP_Name[TOP_fmulx128v32] = "mulps";
  OP_Name[TOP_fmulxx128v32] = "mulps";
  OP_Name[TOP_fmulxxx128v32] = "mulps";
  OP_Name[TOP_fmul128v64] = "mulpd";
  OP_Name[TOP_fmulx128v64] = "mulpd";
  OP_Name[TOP_fmulxx128v64] = "mulpd";
  OP_Name[TOP_fmulxxx128v64] = "mulpd";
  OP_Name[TOP_neg32] = "negl";
  OP_Name[TOP_neg64] = "negq";
  OP_Name[TOP_not32] = "notl";
  OP_Name[TOP_not64] = "notq";
  OP_Name[TOP_or32]   = "orl";
  OP_Name[TOP_orx32]  = "orl";
  OP_Name[TOP_orxx32] = "orl";
  OP_Name[TOP_orxxx32]= "orl";
  OP_Name[TOP_or64]   = "orq";
  OP_Name[TOP_orx64]  = "orq";
  OP_Name[TOP_orxx64]  = "orq";
  OP_Name[TOP_orxxx64] = "orq";
  OP_Name[TOP_ori32] = "orl";
  OP_Name[TOP_ori64] = "orq";
  OP_Name[TOP_pmovmskb128] = "pmovmskb";
  OP_Name[TOP_ror8] = "rorb";
  OP_Name[TOP_ror16] = "rorw";
  OP_Name[TOP_ror32] = "rorl";
  OP_Name[TOP_ror64] = "rorq";
  OP_Name[TOP_rori8] = "rorb";
  OP_Name[TOP_rori16] = "rorw";
  OP_Name[TOP_rori32] = "rorl";
  OP_Name[TOP_rori64] = "rorq";
  OP_Name[TOP_rol8] = "rolb";
  OP_Name[TOP_rol16] = "rolw";
  OP_Name[TOP_rol32] = "roll";
  OP_Name[TOP_rol64] = "rolq";
  OP_Name[TOP_roli8] = "rolb";
  OP_Name[TOP_roli16] = "rolw";
  OP_Name[TOP_roli32] = "roll";
  OP_Name[TOP_roli64] = "rolq";
  OP_Name[TOP_store8]   = "movb";
  OP_Name[TOP_storex8]  = "movb";
  OP_Name[TOP_storexx8] = "movb";
  OP_Name[TOP_store8_n32] = "movb";
  OP_Name[TOP_store16]   = "movw";
  OP_Name[TOP_storex16]  = "movw";
  OP_Name[TOP_storexx16] = "movw";
  OP_Name[TOP_store16_n32] = "movw";
  OP_Name[TOP_store32]  = "movl";
  OP_Name[TOP_storex32] = "movl";
  OP_Name[TOP_storexx32]= "movl";
  OP_Name[TOP_store32_n32]  = "movl";
  OP_Name[TOP_store64]  = "movq";
  OP_Name[TOP_store64_off]  = "movq";
  OP_Name[TOP_storex64] = "movq";
  OP_Name[TOP_storexx64]= "movq";
  OP_Name[TOP_storenti32]  = "movnti";
  OP_Name[TOP_storentix32] = "movnti";
  OP_Name[TOP_storentixx32]= "movnti";
  OP_Name[TOP_storenti64]  = "movnti";
  OP_Name[TOP_storentix64] = "movnti";
  OP_Name[TOP_storentixx64]= "movnti";
  OP_Name[TOP_sar32] = "sarl";
  OP_Name[TOP_sar64] = "sarq";
  OP_Name[TOP_sari32] = "sarl";
  OP_Name[TOP_sari64] = "sarq";
  OP_Name[TOP_shld32]  = "shldl";
  OP_Name[TOP_shldi32] = "shldl";
  OP_Name[TOP_shrd32]  = "shrdl";
  OP_Name[TOP_shrdi32] = "shrdl";
  OP_Name[TOP_shl32] = "shll";
  OP_Name[TOP_shl64] = "shlq";
  OP_Name[TOP_shli32] = "shll";
  OP_Name[TOP_shli64] = "shlq";
  OP_Name[TOP_shr32] = "shrl";
  OP_Name[TOP_shr64] = "shrq";
  OP_Name[TOP_shri32] = "shrl";
  OP_Name[TOP_shri64] = "shrq";
  OP_Name[TOP_sub32] = "subl";
  OP_Name[TOP_sbb32] = "sbbl";
  OP_Name[TOP_sub64] = "subq";
  OP_Name[TOP_subx32] = "subl";
  OP_Name[TOP_subx64] = "subq";
  OP_Name[TOP_subxx32]  = "subl";
  OP_Name[TOP_subxxx32] = "subl";
  OP_Name[TOP_subxx64]  = "subq";
  OP_Name[TOP_subxxx64] = "subq";
  OP_Name[TOP_subi32] = "subl";
  OP_Name[TOP_sbbi32] = "sbbl";
  OP_Name[TOP_subi64] = "subq";
  OP_Name[TOP_subxss] = "subss";
  OP_Name[TOP_subxsd] = "subsd";
  OP_Name[TOP_subxxss]  = "subss";
  OP_Name[TOP_subxxxss] = "subss";
  OP_Name[TOP_subxxsd]  = "subsd";
  OP_Name[TOP_subxxxsd] = "subsd";
  OP_Name[TOP_sub128v8] = "psubb";
  OP_Name[TOP_subx128v8] = "psubb";
  OP_Name[TOP_subxx128v8] = "psubb";
  OP_Name[TOP_subxxx128v8] = "psubb";
  OP_Name[TOP_sub128v16] = "psubw";
  OP_Name[TOP_subx128v16] = "psubw";
  OP_Name[TOP_subxx128v16] = "psubw";
  OP_Name[TOP_subxxx128v16] = "psubw";
  OP_Name[TOP_sub128v32] = "psubd";
  OP_Name[TOP_subx128v32] = "psubd";
  OP_Name[TOP_subxx128v32] = "psubd";
  OP_Name[TOP_subxxx128v32] = "psubd";
  OP_Name[TOP_sub128v64] = "psubq";
  OP_Name[TOP_subx128v64] = "psubq";
  OP_Name[TOP_subxx128v64] = "psubq";
  OP_Name[TOP_subxxx128v64] = "psubq";
  OP_Name[TOP_fsub128v32] = "subps";
  OP_Name[TOP_fsubx128v32] = "subps";
  OP_Name[TOP_fsubxx128v32] = "subps";
  OP_Name[TOP_fsubxxx128v32] = "subps";
  OP_Name[TOP_fsub128v64] = "subpd";
  OP_Name[TOP_fsubx128v64] = "subpd";
  OP_Name[TOP_fsubxx128v64] = "subpd";
  OP_Name[TOP_fsubxxx128v64] = "subpd";
  OP_Name[TOP_test32]   = "testl";
  OP_Name[TOP_testx32]  = "testl";
  OP_Name[TOP_testxx32]  = "testl";
  OP_Name[TOP_testxxx32] = "testl";
  OP_Name[TOP_test64]   = "testq";
  OP_Name[TOP_testx64]  = "testq";
  OP_Name[TOP_testxx64] = "testq";
  OP_Name[TOP_testxxx64]= "testq";
  OP_Name[TOP_testi32]  = "testl";
  OP_Name[TOP_testi64]  = "testq";
  OP_Name[TOP_xor32]   = "xorl";
  OP_Name[TOP_xorx32]  = "xorl";
  OP_Name[TOP_xorxx32] = "xorl";
  OP_Name[TOP_xorxxx32]= "xorl";
  OP_Name[TOP_xor64]   = "xorq";
  OP_Name[TOP_xorx64]  = "xorq";
  OP_Name[TOP_xorxx64] = "xorq";
  OP_Name[TOP_xorxxx64]= "xorq";
  OP_Name[TOP_xori32] = "xorl";
  OP_Name[TOP_xori64] = "xorq";
  OP_Name[TOP_ldss] = "movss";
  OP_Name[TOP_ldss_n32] = "movss";
  OP_Name[TOP_ldssx]  = "movss";
  OP_Name[TOP_ldssxx] = "movss";
  OP_Name[TOP_lddqa] = "movdqa";
  OP_Name[TOP_lddqa_n32] = "movdqa";
  OP_Name[TOP_stdqa] = "movdqa";
  OP_Name[TOP_stdqa_n32] = "movdqa";
  OP_Name[TOP_stntpd]= "movntpd";
  OP_Name[TOP_stntps]= "movntps";
  if ( !Is_Target_SSE3() || ! CG_use_lddqu) {
    OP_Name[TOP_lddqu] = "movdqu";
    OP_Name[TOP_lddqu_n32] = "movdqu";
  }
  else
    OP_Name[TOP_lddqu_n32] = "lddqu";
  OP_Name[TOP_stdqu] = "movdqu";
  OP_Name[TOP_stdqu_n32] = "movdqu";
  OP_Name[TOP_ldlps] = "movlps";
  OP_Name[TOP_ldlps_n32] = "movlps";
  OP_Name[TOP_stlps] = "movlps";
  OP_Name[TOP_stlps_n32] = "movlps";
  OP_Name[TOP_ldlpd] = "movsd";
  OP_Name[TOP_ldlpd_n32] = "movsd";
  OP_Name[TOP_stlpd] = "movsd";
  OP_Name[TOP_stlpd_n32] = "movsd";
  OP_Name[TOP_ldlpsx] = "movlps";
  OP_Name[TOP_stlpsx] = "movlps";
  OP_Name[TOP_ldlpdx] = "movsd";
  OP_Name[TOP_stlpdx] = "movsd";
  OP_Name[TOP_ldlpsxx] = "movlps";
  OP_Name[TOP_stlpsxx] = "movlps";
  OP_Name[TOP_ldlpdxx] = "movsd";
  OP_Name[TOP_stlpdxx] = "movsd";
  OP_Name[TOP_ldhps] = "movhps";
  OP_Name[TOP_ldhps_n32] = "movhps";
  OP_Name[TOP_sthps] = "movhps";
  OP_Name[TOP_sthps_n32] = "movhps";
  OP_Name[TOP_ldhpd] = "movhpd";
  OP_Name[TOP_ldhpd_n32] = "movhpd";
  OP_Name[TOP_sthpd] = "movhpd";
  OP_Name[TOP_sthpd_n32] = "movhpd";
  OP_Name[TOP_lddqax] = "movdqa";
  OP_Name[TOP_stdqax] = "movdqa";
  OP_Name[TOP_stntpdx]= "movntpd";
  OP_Name[TOP_stntpsx]= "movntps";
  if ( !Is_Target_SSE3() || ! CG_use_lddqu)
    OP_Name[TOP_lddqux] = "movdqu";
  else
    OP_Name[TOP_lddqux] = "lddqu";
  OP_Name[TOP_stdqux] = "movdqu";
  OP_Name[TOP_ldhpsx] = "movhps";
  OP_Name[TOP_sthpsx] = "movhps";
  OP_Name[TOP_ldhpdx] = "movhpd";
  OP_Name[TOP_sthpdx] = "movhpd";
  OP_Name[TOP_lddqaxx] = "movdqa";
  OP_Name[TOP_stdqaxx] = "movdqa";
  OP_Name[TOP_stntpdxx]= "movntpd";
  OP_Name[TOP_stntpsxx]= "movntps";
  if ( !Is_Target_SSE3() || ! CG_use_lddqu)
    OP_Name[TOP_lddquxx] = "movdqu";
  else
    OP_Name[TOP_lddquxx] = "lddqu";
  OP_Name[TOP_stdquxx] = "movdqu";
  OP_Name[TOP_ldhpsxx] = "movhps";
  OP_Name[TOP_sthpsxx] = "movhps";
  OP_Name[TOP_ldhpdxx] = "movhpd";
  OP_Name[TOP_sthpdxx] = "movhpd";
  OP_Name[TOP_staps] = "movaps";
  OP_Name[TOP_staps_n32] = "movaps";
  OP_Name[TOP_stapd] = "movapd";
  OP_Name[TOP_stapd_n32] = "movapd";
  OP_Name[TOP_stupd] = "movupd";
  OP_Name[TOP_stups] = "movups";
  OP_Name[TOP_ldaps] = "movaps";
  OP_Name[TOP_ldaps_n32] = "movaps";
  OP_Name[TOP_ldapd] = "movapd";
  OP_Name[TOP_ldapd_n32] = "movapd";
  OP_Name[TOP_stapsx] = "movaps";
  OP_Name[TOP_stapdx] = "movapd";
  OP_Name[TOP_ldapsx] = "movaps";
  OP_Name[TOP_ldapdx] = "movapd";
  OP_Name[TOP_stapsxx] = "movaps";
  OP_Name[TOP_stapdxx] = "movapd";
  OP_Name[TOP_ldapsxx] = "movaps";
  OP_Name[TOP_ldapdxx] = "movapd";
  OP_Name[TOP_ldupd] = "movupd";
  OP_Name[TOP_ldupd_n32] = "movupd";
  OP_Name[TOP_ldups] = "movups";
  OP_Name[TOP_ldups_n32] = "movups";
  OP_Name[TOP_movss] = "movaps";
  OP_Name[TOP_movdq] = "movdqa";
  OP_Name[TOP_movg2x] = "movd";
  OP_Name[TOP_movg2x64] = "movd";
  OP_Name[TOP_movx2g] = "movd";
  OP_Name[TOP_movx2g64] = "movd";
  OP_Name[TOP_stss_n32]   = "movss";
  OP_Name[TOP_stss]   = "movss";
  OP_Name[TOP_stssx]  = "movss";
  OP_Name[TOP_stssxx] = "movss";

  //SSSE4 instructions 
  OP_Name[TOP_stntss]   = "movntss";
  OP_Name[TOP_stntssx]  = "movntss";
  OP_Name[TOP_stntssxx] = "movntss";
  OP_Name[TOP_stntsd]   = "movntsd";
  OP_Name[TOP_stntsdx]  = "movntsd";
  OP_Name[TOP_stntsdxx] = "movntsd";
  
  OP_Name[TOP_fmov] = "fld";
  OP_Name[TOP_ld8_abs]  = "movabsb";
  OP_Name[TOP_ld16_abs] = "movabsw";
  OP_Name[TOP_ld32_abs] = "movabsl";
  OP_Name[TOP_ld64_abs] = "movabsq";
  OP_Name[TOP_store8_abs]  = "movabsb";
  OP_Name[TOP_store16_abs] = "movabsw";
  OP_Name[TOP_store32_abs] = "movabsl";
  OP_Name[TOP_store64_abs] = "movabsq";

  OP_Name[TOP_ld32_gs_seg_off]	= "movl";
  OP_Name[TOP_ld64_fs_seg_off]	= "movq";

  OP_Name[TOP_cvtsd2ss_x]   = "cvtsd2ss";
  OP_Name[TOP_cvtsd2ss_xx]  = "cvtsd2ss";
  OP_Name[TOP_cvtsd2ss_xxx] = "cvtsd2ss";
  OP_Name[TOP_cvtsi2sd_x]   = "cvtsi2sd";
  OP_Name[TOP_cvtsi2sd_xx]  = "cvtsi2sd";
  OP_Name[TOP_cvtsi2sd_xxx] = "cvtsi2sd";
  OP_Name[TOP_cvtsi2ss_x]   = "cvtsi2ss";
  OP_Name[TOP_cvtsi2ss_xx]  = "cvtsi2ss";
  OP_Name[TOP_cvtsi2ss_xxx] = "cvtsi2ss";
  OP_Name[TOP_cvtsi2sdq_x]  = "cvtsi2sdq";
  OP_Name[TOP_cvtsi2sdq_xx] = "cvtsi2sdq";
  OP_Name[TOP_cvtsi2sdq_xxx]= "cvtsi2sdq";
  OP_Name[TOP_cvtsi2ssq_x]  = "cvtsi2ssq";
  OP_Name[TOP_cvtsi2ssq_xx] = "cvtsi2ssq";
  OP_Name[TOP_cvtsi2ssq_xxx]= "cvtsi2ssq";
  
  OP_Name[TOP_cvtdq2pd_x] = "cvtdq2pd";
  OP_Name[TOP_cvtdq2ps_x] = "cvtdq2ps";
  OP_Name[TOP_cvtps2pd_x] = "cvtps2pd";
  OP_Name[TOP_cvtpd2ps_x] = "cvtpd2ps";
  OP_Name[TOP_cvttps2dq_x] = "cvttps2dq";
  OP_Name[TOP_cvttpd2dq_x] = "cvttpd2dq";
  OP_Name[TOP_cvtdq2pd_xx] = "cvtdq2pd";
  OP_Name[TOP_cvtdq2ps_xx] = "cvtdq2ps";
  OP_Name[TOP_cvtps2pd_xx] = "cvtps2pd";
  OP_Name[TOP_cvtpd2ps_xx] = "cvtpd2ps";
  OP_Name[TOP_cvttps2dq_xx] = "cvttps2dq";
  OP_Name[TOP_cvttpd2dq_xx] = "cvttpd2dq";
  OP_Name[TOP_cvtdq2pd_xxx] = "cvtdq2pd";
  OP_Name[TOP_cvtdq2ps_xxx] = "cvtdq2ps";
  OP_Name[TOP_cvtps2pd_xxx] = "cvtps2pd";
  OP_Name[TOP_cvtpd2ps_xxx] = "cvtpd2ps";
  OP_Name[TOP_cvttps2dq_xxx] = "cvttps2dq";
  OP_Name[TOP_cvttpd2dq_xxx] = "cvttpd2dq";
//**********************************************************
// For barcelona (bug 13108)
// (1) "movlpd reg, mem"  ==> "movsd reg, mem"
// (2) "movsd reg, reg"   ==> "movapd reg, reg"
// NOTE: there are regardless of CG_use_movlpd TRUE or FALSE
//***********************************************************
  if (CG_use_movlpd &&
      !Is_Target_Pentium4() &&
      !Is_Target_EM64T() &&
      !Is_Target_Core() &&
      !Is_Target_Wolfdale() &&
      !Is_Target_Barcelona()){// bug 10295
    // Use movlpd only for loads.  Bug 5809.
    OP_Name[TOP_ldsd] = "movlpd";
    OP_Name[TOP_ldsd_n32] = "movlpd";
    OP_Name[TOP_stsdx]  = "movsd";
    OP_Name[TOP_stsdxx] = "movsd";
    OP_Name[TOP_movsd] = "movsd"; 
    OP_Name[TOP_ldsdx]  = "movlpd";
    OP_Name[TOP_ldsdxx] = "movlpd";
    OP_Name[TOP_stsd] = "movsd";
    OP_Name[TOP_stsd_n32] = "movsd";
    OP_Name[TOP_storelpd] = "movlpd";
  }
  else{
    OP_Name[TOP_ldsd] = "movsd";
    OP_Name[TOP_ldsd_n32] = "movsd";
    OP_Name[TOP_stsdx]  = "movsd";
    OP_Name[TOP_stsdxx] = "movsd";
    OP_Name[TOP_ldsdx]  = "movsd";
    OP_Name[TOP_ldsdxx] = "movsd";
    OP_Name[TOP_stsd] = "movsd";
    OP_Name[TOP_stsd_n32] = "movsd";
    OP_Name[TOP_storelpd] = "movsd";
    if (Is_Target_Barcelona() ||
	Is_Target_EM64T()     || // em64t
        Is_Target_Wolfdale()  ||
	Is_Target_Core()) {	 // use movapd for woodcrest for bug 11548
      OP_Name[TOP_movsd] = "movapd";  
    }
  }

  OP_Name[TOP_lock_add8] = "addb";
  OP_Name[TOP_lock_add16] = "addw";
  OP_Name[TOP_lock_add32] = "addl";
  OP_Name[TOP_lock_adc32] = "adcl";
  OP_Name[TOP_lock_add64] = "addq";

  OP_Name[TOP_lock_cmpxchg8] = "cmpxchgb";
  OP_Name[TOP_lock_cmpxchg16] = "cmpxchgw";
  OP_Name[TOP_lock_cmpxchg32] = "cmpxchgl";
  OP_Name[TOP_lock_cmpxchg64] = "cmpxchgq";

  OP_Name[TOP_lock_and8] = "andb";
  OP_Name[TOP_lock_and16] = "andw";
  OP_Name[TOP_lock_and32] = "andl";
  OP_Name[TOP_lock_and64] = "andq";

  OP_Name[TOP_lock_or8] = "orb";
  OP_Name[TOP_lock_or16] = "orw";
  OP_Name[TOP_lock_or32] = "orl";
  OP_Name[TOP_lock_or64] = "orq";

  OP_Name[TOP_lock_xor8] = "xorb";
  OP_Name[TOP_lock_xor16] = "xorw";
  OP_Name[TOP_lock_xor32] = "xorl";
  OP_Name[TOP_lock_xor64] = "xorq";

  OP_Name[TOP_lock_sub8] = "subb";
  OP_Name[TOP_lock_sub16] = "subw";
  OP_Name[TOP_lock_sub32] = "subl";
  OP_Name[TOP_lock_sub64] = "subq";

  OP_Name[TOP_lock_xadd8] = "xaddb";
  OP_Name[TOP_lock_xadd16] = "xaddw";
  OP_Name[TOP_lock_xadd32] = "xaddl";
  OP_Name[TOP_lock_xadd64] = "xaddq";

  OP_Name[TOP_bsf32] = "bsfl";
  OP_Name[TOP_bsf64] = "bsfq";
  OP_Name[TOP_bsr32] = "bsrl";
  OP_Name[TOP_bsr64] = "bsrq";

  OP_Name[TOP_ld64_2m] = "movq";
  OP_Name[TOP_ld64_2m_n32] = "movq";
  OP_Name[TOP_store64_fm] = "movq";
  OP_Name[TOP_store64_fm_n32] = "movq";
  OP_Name[TOP_storent64_fm] = "movntq";
  OP_Name[TOP_ld64_2sse] = "movq";
  OP_Name[TOP_ld64_2sse_n32] = "movq";
  OP_Name[TOP_store64_fsse] = "movq";
  OP_Name[TOP_store64_fsse_n32] = "movq";
  OP_Name[TOP_mov64_m] = "movq";
  OP_Name[TOP_add64v8] = "paddb";
  OP_Name[TOP_add64v16] = "paddw";
  OP_Name[TOP_add64v32] = "paddd";
  OP_Name[TOP_sub64v8] = "psubb";
  OP_Name[TOP_sub64v16] = "psubw";
  OP_Name[TOP_sub64v32] = "psubd";
  OP_Name[TOP_punpckl64v8] = "punpcklbw",
  OP_Name[TOP_punpckl64v16] = "punpcklwd",
  OP_Name[TOP_punpckl64v32] = "punpckldq";
  OP_Name[TOP_max64v8] = "pmaxub";
  OP_Name[TOP_max64v16] = "pmaxsw";
  OP_Name[TOP_min64v8] = "pminub";
  OP_Name[TOP_min64v16] = "pminsw";
  OP_Name[TOP_movi32_2m] = "movd";
  OP_Name[TOP_movi64_2m] = "movd";
  OP_Name[TOP_movm_2i32] = "movd";
  OP_Name[TOP_movm_2i64] = "movd";
  OP_Name[TOP_psrlq128v64] = "psrlq";
  OP_Name[TOP_storenti128] = "movntdq";
  OP_Name[TOP_pshufw64v16] = "pshufw";
  OP_Name[TOP_psllw_mmx] = "psllw";
  OP_Name[TOP_pslld_mmx] = "pslld";
  OP_Name[TOP_psrlw_mmx] = "psrlw";
  OP_Name[TOP_psrld_mmx] = "psrld";
  OP_Name[TOP_psraw_mmx] = "psraw";
  OP_Name[TOP_psrad_mmx] = "psrad";
  OP_Name[TOP_pand_mmx] = "pand";
  OP_Name[TOP_pandn_mmx] = "pandn";
  OP_Name[TOP_por_mmx] = "por";
  OP_Name[TOP_pxor_mmx] = "pxor";
  OP_Name[TOP_extrq] = "extrq";
  OP_Name[TOP_insertq] = "insertq";

  return;
}


/* bug#1592 */
// bug 3699
#define NAME_LEN 8192

enum OPND_REG { BYTE_REG = 0, WORD_REG, DWORD_REG, QWORD_REG, SSE2_REG };

static enum OPND_REG Get_Opnd_Reg( TOP topcode, int opnd )
{
  int num_bits = 0;

  if( opnd >= 0 ){
    if( opnd == TOP_Find_Operand_Use(topcode,OU_base) ||
	opnd == TOP_Find_Operand_Use(topcode,OU_index) ){
      num_bits = Pointer_Size * 8;

    } else {
      // For regular operands.
      const ISA_OPERAND_INFO* oinfo = ISA_OPERAND_Info(topcode);
      const ISA_OPERAND_VALTYP *otype = ISA_OPERAND_INFO_Operand(oinfo, opnd);

      num_bits = ISA_OPERAND_VALTYP_Size(otype);
    }

  } else {  // opnd < 0
    const ISA_OPERAND_INFO* oinfo = ISA_OPERAND_Info(topcode);
    const ISA_OPERAND_VALTYP* otype = ISA_OPERAND_INFO_Result(oinfo, 0);

    num_bits = ISA_OPERAND_VALTYP_Size(otype);
  }

  /* We might need to fix up isa_operands.cxx later.
     Also, don't count on TN_size().
   */
  if( Is_Target_32bit()           &&
      ( ( TOP_is_cond_move( topcode ) &&
	  !TOP_is_flop( topcode ) ) ||
	TOP_is_ijump( topcode ) ) ) {
    num_bits = 32;
  }

  switch( num_bits ){
  case 8:   return BYTE_REG;
  case 16:  return WORD_REG;
  case 32:  return DWORD_REG;
  case 64:  return QWORD_REG;
  case 128: return SSE2_REG;
  default:
    FmtAssert( false, ("NYI") );
  }

  return SSE2_REG;
}


static void Str_Prepend( char* str, char c )
{
  for( int i = strlen(str) + 1; i > 0; i-- ){
    str[i] = str[i-1];
  }

  str[0] = c;
}

static const char* int_reg_names[4][16] = {
  /* BYTE_REG: low 8-bit */
  { "%al", "%bl", "%bpl", "%spl", "%dil", "%sil", "%dl", "%cl",
    "%r8b",  "%r9b",  "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b" },
  /* WORD_REG: 16-bit */
  { "%ax", "%bx", "%bp", "%sp", "%di", "%si", "%dx", "%cx",
    "%r8w",  "%r9w",  "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w" },
  /* DWORD_REG: 32-bit */
  { "%eax", "%ebx", "%ebp", "%esp", "%edi", "%esi", "%edx", "%ecx",
    "%r8d",  "%r9d",  "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d" },
  /* QWORD_REG: 64-bit */
  { "%rax", "%rbx", "%rbp", "%rsp", "%rdi", "%rsi", "%rdx", "%rcx",
    "%r8",  "%r9",  "%r10", "%r11", "%r12", "%r13", "%r14", "%r15" },
};

static void Adjust_Opnd_Name( OP* op, int opnd, char* name )
{
  const TOP topcode = OP_code( op );

  if( TOP_is_ijump( topcode ) &&
      ( opnd == OP_find_opnd_use(op,OU_target) ||
	opnd == OP_find_opnd_use(op,OU_offset) ) ){
    if ( Is_Target_32bit() ) { // Bug 4666
      const enum OPND_REG opnd_reg = Get_Opnd_Reg( topcode, opnd );
      const ISA_REGISTER_CLASS rc = ISA_REGISTER_CLASS_integer;
      
      for( REGISTER reg = REGISTER_MIN; 
	   reg <= REGISTER_CLASS_last_register( rc ); reg++ ){
	const char* n = REGISTER_name( rc, reg );
	if( strcmp( n, name ) == 0 ){
	  strcpy( name, int_reg_names[opnd_reg][reg-REGISTER_MIN] );
	  break;
	}
      }      
    }
    Str_Prepend( name, '*' );
    return;
  }

  const ISA_OPERAND_INFO* oinfo = ISA_OPERAND_Info(topcode);
  const ISA_OPERAND_VALTYP* vtype = opnd >= 0
    ? ISA_OPERAND_INFO_Operand(oinfo, opnd)
    : ISA_OPERAND_INFO_Result(oinfo, 0);
    
  // Handle case where opnd is an imm. field.

  if( ISA_OPERAND_VALTYP_Is_Literal(vtype) ){
    // Bug 578
#if defined(BUILD_OS_DARWIN)
    // Mach-O linker evidently handles this automatically
#else /* defined(BUILD_OS_DARWIN) */
    // Add a reference to the PLT under -fPIC compilation.
    if ( Gen_PIC_Shared &&  
	 !TN_is_label( OP_opnd(op,0) ) &&
	 ( topcode == TOP_call || 
	   // tail-call optimization 
	   ( topcode == TOP_jmp && !TN_is_label( OP_opnd( op, 0))))) {
      ST* function = TN_var(OP_opnd(op, 0));
      if ( function && !ST_is_export_local(function) )
        strcat( name, "@PLT" );
    }
#endif /* defined(BUILD_OS_DARWIN) */

    if (name[0] != '$' &&   /* A '$' has been added by CGEMIT_Relocs_In_Asm() */
	(opnd != OP_find_opnd_use(op,OU_offset) &&
	 opnd != OP_find_opnd_use(op,OU_scale)  &&
	 opnd != OP_find_opnd_use(op,OU_target)) &&
	// Constant is not 32-bit ABI thread-local symbol offset.
	(!(Is_Target_32bit() &&
	   (TN_relocs(OP_opnd(op, opnd)) == TN_RELOC_X8664_TPOFF32 ||
	    TN_relocs(OP_opnd(op, opnd)) == TN_RELOC_X8664_GOTTPOFF)))) {
      Str_Prepend(name, '$');
    }

    // Bug 506
    {
      char *nl = strstr(name, "\n");
      if (nl) {
	if (strstr (name, ")") == NULL)
	  name[nl - name] = '\0';
	else 
	  { name[nl - name] = ')'; name[nl - name + 1] = '\0'; }
      }
    }

    return;
  }

  // Return if the opnd is not a register.

  if( !ISA_OPERAND_VALTYP_Is_Register(vtype) )
    return;

  if( ISA_OPERAND_VALTYP_Register_Class(vtype) == ISA_REGISTER_CLASS_integer ){
    const enum OPND_REG opnd_reg = Get_Opnd_Reg( topcode, opnd );
    const ISA_REGISTER_CLASS rc = ISA_REGISTER_CLASS_integer;

    for( REGISTER reg = REGISTER_MIN; reg <= REGISTER_CLASS_last_register( rc ); reg++ ){
      const char* n = REGISTER_name( rc, reg );
      if( strcmp( n, name ) == 0 ){
	strcpy( name, int_reg_names[opnd_reg][reg-REGISTER_MIN] );
	return;
      }
    }
  } // ISA_REGISTER_CLASS_integer

  if( ISA_OPERAND_VALTYP_Register_Class(vtype) == ISA_REGISTER_CLASS_x87 ){
    extern int Get_OP_stack_reg( OP*, int );
    sprintf( name, "%%st(%d)", Get_OP_stack_reg( op, opnd ) );
    return;
  } // ISA_REGISTER_CLASS_x87
}


// Return x86 segment prefix override.
static const char *
Segment_Prefix (OP *op)
{
  if (CGTARG_Is_Thread_Local_Memory_OP(op)) {
    return Is_Target_32bit() ? "%gs:" : "%fs:";
  }
  return "";
}

INT CGEMIT_Print_Inst( OP* op, const char* result[], const char* opnd[], FILE* f )
{
  Init_OP_Name();

  char result_name[2][NAME_LEN];  // It is also the first opnd name.
  char opnd_name[9][NAME_LEN];
  const TOP topcode = OP_code(op);
  const ISA_PRINT_INFO* pinfo = ISA_PRINT_Info(topcode);
  char op_name[NAME_LEN];

  FmtAssert( pinfo != NULL, ("no ISA_PRINT_INFO for %s", TOP_Name(topcode)) );

  /* STEP 1:
     Collect all the opnds and result info. */

  const char* p = OP_Name[topcode] == NULL ? TOP_Name(topcode) : OP_Name[topcode];

  if (OP_prefix_lock(op))
    sprintf( op_name, "lock %s", p );
  else
    strcpy( op_name, p );


  INT i = 0, opnd_i = 0, result_i = 0;

  while( TRUE ){
    const INT comp = ISA_PRINT_INFO_Comp(pinfo, i);

    switch( comp ){

    case ISA_PRINT_COMP_opnd:
    case ISA_PRINT_COMP_opnd+1:
    case ISA_PRINT_COMP_opnd+2:
    case ISA_PRINT_COMP_opnd+3:
    case ISA_PRINT_COMP_opnd+4:
    case ISA_PRINT_COMP_opnd+5:
      Is_True(10 + strlen(opnd[comp - ISA_PRINT_COMP_opnd])
	        < sizeof(opnd_name[opnd_i]),
	      ("buffer size is too small") );
      strcpy(opnd_name[opnd_i], opnd[comp - ISA_PRINT_COMP_opnd]);
      Adjust_Opnd_Name(op, comp - ISA_PRINT_COMP_opnd, opnd_name[opnd_i]);
      opnd_i++;
      break;

    case ISA_PRINT_COMP_result:
    case ISA_PRINT_COMP_result+1:
      Is_True(10 + strlen(result[comp - ISA_PRINT_COMP_result])
	        < sizeof(result_name[result_i]),
	      ("buffer size is too small"));
      strcpy(result_name[result_i], result[comp - ISA_PRINT_COMP_result]);
      Adjust_Opnd_Name(op, -1, result_name[result_i]);
      result_i++;
      break;

    case ISA_PRINT_COMP_segment:
      {
	const char *segment_prefix = Segment_Prefix(op);
	Is_True(10 + strlen(segment_prefix) < sizeof(opnd_name[opnd_i]),
		("buffer size is too small"));
	strcpy(opnd_name[opnd_i], segment_prefix);
	opnd_i++;
      }
      break;

    case ISA_PRINT_COMP_name:
    case ISA_PRINT_COMP_end:
      break;

    default:
      FmtAssert(false, ("Unhandled listing component %d for %s",
			comp, TOP_Name(topcode)));
    }

    i++;
    if( comp == ISA_PRINT_COMP_end )
      break;
  }

  /* STEP 2:
     Print out the info. */

  for( i = 0; i < result_i; i++ ){
    strcpy( opnd_name[opnd_i], result_name[i] );
    opnd_i++;
  }

  INT st = fprintf( f, ISA_PRINT_INFO_Format(pinfo),
		    op_name,
		    opnd_name[0], opnd_name[1], opnd_name[2], opnd_name[3], 
		    opnd_name[4], opnd_name[5], opnd_name[6], opnd_name[7],
		    opnd_name[8] );

  FmtAssert( st != -1,
             ("fprintf failed: not enough disk space while compiling %s",
              Orig_Src_File_Name ? Orig_Src_File_Name : Src_File_Name ));

  return st;
}


void CGEMIT_Setup_Ctrl_Register( FILE* f )
{
  // Set x87 precision (32/64/80-bit).  Bug 4327.
  int x87_mask;
  switch (Target_x87_precision()) {
    case 32:  x87_mask = 0; break;	// single precision
    case 64:  x87_mask = 0x200; break;	// double precision
    case 80:  x87_mask = 0x300; break;	// double-extended precision
    default:  FmtAssert(FALSE,
			("CGEMIT_Setup_Ctrl_Register: invalid x87 precision"));
  }
  // If trapuv, turn on exception for invalid operands.
  unsigned int x86_cntrl_mask = DEBUG_Trap_Uv ? 0xfcfe : 0xfcff;
  if( Is_Target_64bit() ){
    fprintf( f, "\taddq    $-8,%%rsp\n"             );
    fprintf( f, "\tfnstcw  (%%rsp)\n"               );
    fprintf( f, "\tandw    $0x%x,(%%rsp)\n", x86_cntrl_mask);
    fprintf( f, "\torw     $%d,(%%rsp)\n", x87_mask );
    fprintf( f, "\tfldcw   (%%rsp)\n"               );
    fprintf( f, "\taddq    $8,%%rsp\n"              );
  } else {
    fprintf( f, "\taddl    $-4,%%esp\n"             );
    fprintf( f, "\tfnstcw  (%%esp)\n"               );
    fprintf( f, "\tandw    $0x%x,(%%esp)\n", x86_cntrl_mask);
    fprintf( f, "\torw     $%d,(%%esp)\n", x87_mask );
    fprintf( f, "\tfldcw   (%%esp)\n"               );
    fprintf( f, "\taddl    $4,%%esp\n"              );
  }

  BOOL is_MAIN__ = !strcmp(Cur_PU_Name, "MAIN__");

  if (IEEE_Arithmetic <= IEEE_ACCURATE &&
      !is_MAIN__ &&
      SIMD_IMask &&
      SIMD_DMask &&
      SIMD_ZMask &&
      SIMD_OMask &&
      SIMD_UMask &&
      SIMD_PMask &&
      !DEBUG_Trap_Uv)
    return;

  /* The following sequence of code is used to turn off
     hardware underflow exception handling.
   */

  const int mask = 32768 ;
  const int simd_imask = 0xff7f; // bit 7 : f f 0111 f
  const int simd_dmask = 0xfeff; // bit 8 : f 1110 f f
  const int simd_zmask = 0xfdff; // bit 9 : f 1101 f f
  const int simd_omask = 0xfbff; // bit 10: f 1011 f f
  const int simd_umask = 0xf7ff; // bit 11: f 0111 f f
  const int simd_pmask = 0xefff; // bit 12: 1110 f f f
  // bits 16..31 are 0, and must remain 0

  if( Is_Target_64bit() ){
    fprintf( f, "\t%s\n", "addq    $-8,%rsp"      );
    fprintf( f, "\t%s\n", "stmxcsr (%rsp)"        );
    if (IEEE_Arithmetic > IEEE_ACCURATE)
      fprintf( f, "\torq $%d, (%%rsp)\n", mask);
    else if (is_MAIN__)				// bug 8926
      fprintf( f, "\tandq $%d, (%%rsp)\n", ~mask);
    if ( !SIMD_IMask || DEBUG_Trap_Uv)		// trap invalid operands
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_imask );
    if ( !SIMD_DMask )
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_dmask );
    if ( !SIMD_ZMask )
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_zmask );
    if ( !SIMD_OMask )
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_omask );
    if ( !SIMD_UMask )
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_umask );
    if ( !SIMD_PMask )
      fprintf( f, "\tandq $%d, (%%rsp)\n", simd_pmask );
    fprintf( f, "\t%s\n", "ldmxcsr (%rsp)"        );
    fprintf( f, "\t%s\n", "addq    $8,%rsp"       );

  } else {
    fprintf( f, "\t%s\n", "addl    $-8,%esp"      );
    fprintf( f, "\t%s\n", "stmxcsr (%esp)"        );
    if (IEEE_Arithmetic > IEEE_ACCURATE)
      fprintf( f, "\torl $%d, (%%esp)\n", mask);
    else if (is_MAIN__)				// bug 8926
      fprintf( f, "\tandl $%d, (%%esp)\n", ~mask);
    if ( !SIMD_IMask || DEBUG_Trap_Uv)		// trap invalid operands
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_imask );
    if ( !SIMD_DMask )
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_dmask );
    if ( !SIMD_ZMask )
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_zmask );
    if ( !SIMD_OMask )
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_omask );
    if ( !SIMD_UMask )
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_umask );
    if ( !SIMD_PMask)
      fprintf( f, "\tandl $%d, (%%esp)\n", simd_pmask );
    fprintf( f, "\t%s\n", "ldmxcsr (%esp)"        );
    fprintf( f, "\t%s\n", "addl    $8,%esp"       );
  }

  return;
}

// Emits function used to retrieve the Instruction Pointer (bug 9675)
void CGEMIT_Setup_IP_Calc (void)
{
  fprintf (Asm_File, "\n\t.section .gnu.linkonce.t.%s,\"ax\",@progbits\n", ip_calc_funcname);
  fprintf (Asm_File, "\t.globl %s\n", ip_calc_funcname);
  fprintf (Asm_File, "\t.hidden %s\n", ip_calc_funcname);
  fprintf (Asm_File, "\t.type %s, @function\n", ip_calc_funcname);
  fprintf (Asm_File, "%s:\n", ip_calc_funcname);
  fprintf (Asm_File, "\tmovl (%%esp),%%ebx\n");
  fprintf (Asm_File, "\tret\n");
}
