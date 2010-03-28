/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


#include <stdio.h>
#include <stdlib.h>
#include "libelf/libelf.h"
#include <list>
#include "elf_stuff.h"
#include <elfaccess.h>

#include "defs.h"
#include "erglob.h"
#include "glob.h"
#include "flags.h"
#include "tracing.h"
#include "config.h"
#include "config_asm.h"
#include "be_util.h"
#include "cgir.h"
#include "register.h"
#include "tn_map.h"
#include "em_elf.h"
#include "em_dwarf.h"
#include "cgtarget.h"
#include "calls.h"
#include "cgemit.h"
#include "data_layout.h"
#include "cgdwarf_targ.h"
#include "whirl2ops.h"

// call per-PU
void 
Init_Unwind_Info (BOOL trace)
{
  return;
}

void 
Finalize_Unwind_Info(void)
{
  return;
}


void 
Emit_Unwind_Directives_For_OP(OP *op, FILE *f)
{
  return;
}


/* construct the fde for the current procedure. */
extern Dwarf_P_Fde
Build_Fde_For_Proc (Dwarf_P_Debug dw_dbg, BB *firstbb,
		    Dwarf_Unsigned begin_label,
		    Dwarf_Unsigned end_label,
		    Dwarf_Unsigned adjustsp_label,
		    Dwarf_Unsigned callee_saved_reg,
		    INT32     new_cfa_offset,
		    INT32     end_offset,
		    // The following two arguments need to go away
		    // once libunwind gives us an interface that
		    // supports symbolic ranges.
		    INT       low_pc,
		    INT       high_pc)
{
  Dwarf_Error dw_error;
  Dwarf_P_Fde fde;

  if ( ! CG_emit_unwind_info) return NULL;

  fde = dwarf_new_fde (dw_dbg, &dw_error);

  // If there was no SP adjustment, then no need to generate dummy
  // DWARF instructions (bug 12568). In such a scenario, the FDE
  // should only contain the start location, and the address range.
  if (adjustsp_label == 0)
    return fde;

  // Generate FDE instructions
  dwarf_add_fde_inst (fde, DW_CFA_advance_loc4,
                      begin_label, adjustsp_label, &dw_error);

  dwarf_add_fde_inst (fde, DW_CFA_def_cfa_offset,
                      new_cfa_offset, 0x0, &dw_error);

  if (Cgdwarf_Num_Callee_Saved_Regs() && callee_saved_reg) {
    INT num = Cgdwarf_Num_Callee_Saved_Regs();
    // data alignment factor
    INT d_align = 4;
    dwarf_add_fde_inst (fde, DW_CFA_advance_loc4,
                        adjustsp_label,
                        callee_saved_reg, &dw_error);
    for (INT i = num - 1; i >= 0; i --) {
      TN* tn = Cgdwarf_Nth_Callee_Saved_Reg(i);
      ST* sym = Cgdwarf_Nth_Callee_Saved_Reg_Location(i);
      PREG_NUM reg_id = TN_To_Assigned_PREG (tn);
      Dwarf_Signed ofst = (Dwarf_Signed)(ST_ofst(sym) - new_cfa_offset)/d_align;
      dwarf_add_fde_inst_with_signed_offset (fde,
                                             DW_CFA_offset_extended_sf,
                                             reg_id,
                                             ofst,
                                             &dw_error);
    }
  }

  return fde;
}


void
Check_Dwarf_Rel(const Elf32_Rel &current_reloc)
{
  FmtAssert(REL32_type(current_reloc) == R_IA_64_DIR32MSB,
	    ("Unimplemented 32-bit relocation type %d",
	     REL32_type(current_reloc)));
}

void
Check_Dwarf_Rel(const Elf64_AltRel &current_reloc)
{
  FmtAssert(REL64_type(current_reloc) == R_IA_64_DIR64MSB,
	    ("Unimplemented 64-bit relocation type %d",
	     REL64_type(current_reloc)));
}

void
Check_Dwarf_Rela(const Elf64_AltRela &current_reloc)
{
  FmtAssert(FALSE,
	    ("Unimplemented 64-bit relocation type %d",
	     REL64_type(current_reloc)));
}

void
Check_Dwarf_Rela(const Elf32_Rela &current_reloc)
{
  FmtAssert(FALSE,
	    ("Unimplemented 32-bit relocation type %d",
	     REL32_type(current_reloc)));
}
static const char *drop_these[] = {
      // Assembler generates .debug_line from directives itself, so we
      // don't output it.
	".debug_line",
     // gdb does not use the MIPS sections
     // debug_weaknames, etc.
	".debug_weaknames",
	".debug_varnames",
	".debug_typenames",
	".debug_funcnames",
	0
};
// return TRUE if we want to emit the section (IA-64).
// return FALSE if we do not want to for IA-64.
extern BOOL Is_Dwarf_Section_To_Emit(const char *name)
{

	for(int  i = 0; drop_these[i]; ++i) {
	  if(strcmp(name,drop_these[i]) == 0) {
	    return FALSE;
	  }
	}
        if (Debug_Level < 1 && strncmp(name, ".debug_", 7) == 0)
          return FALSE;
        return TRUE;
}

