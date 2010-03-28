/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


/*

  xlate_expand_reg.c

  $Revision$

  These are used little: mainly by elfdump.

*/

#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_expand_reg_info_xtnd = _xlate_expand_reg_info_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_expand_reg_info = _xlate_expand_reg_info
#endif

/* obsolescent */
int
xlate_expand_reg_info(xlate_table_con tab,
	Elf64_Xword * num_instrs,
	xlate_reg_instr ** instructions)
{

        Dwarf_Regtable regtab;
	xlate_reg_instr2 *xlr2;
	xlate_reg_instr *xlr;
	int retstatus;

        if(tab->xc_valid_table != VALID_TABLE_MAGIC) {
                return XLATE_TB_STATUS_INVALID_TABLE;
        }



        retstatus = _xlate_expand_reg_info_internal(tab,
                ALL_INSTRUCTIONS,
                /* pc */0,
                &regtab,
                num_instrs,
		&xlr2);

	if(retstatus == XLATE_TB_STATUS_NO_ERROR) {
	  Uword max = *num_instrs;
	  Uword ct;
          xlr = (xlate_reg_instr *)malloc( 
		(max) * sizeof(xlate_reg_instr));
	  if(xlr == 0) {
		return XLATE_TB_STATUS_ALLOC_FAIL;
	  }
	  for(ct = 0; ct < max; ++ct) {
		xlr[ct].sr_op = xlr2[ct].sr_op;
		xlr[ct].sr_val1 = xlr2[ct].sr_val1;
		xlr[ct].sr_val2 = xlr2[ct].sr_val2;
	  }
	  *instructions = xlr;
	  free(xlr2);
	}
	return retstatus;
}

