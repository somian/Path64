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

  xlate_get_reg.c

  $Revision$


*/

#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_get_reg_rule_xtnd = _xlate_get_reg_rule_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_get_reg_rule = _xlate_get_reg_rule
#endif


int
xlate_get_reg_rule(xlate_table_con tab,
	Elf64_Addr pc,
	Elf32_Word reg,
	Elf32_Word  *rule_register,
	Elf64_Xword *rule_offset,
        Elf32_Word  *rule_is_offset)
{
	int retstatus;
	Dwarf_Regtable regtab;
   	if(tab->xc_valid_table != VALID_TABLE_MAGIC) {
        	return XLATE_TB_STATUS_INVALID_TABLE;
   	}


	

	retstatus = _xlate_expand_reg_info_internal(tab,
		REG_RULE_AT_PC,
                pc,
        	&regtab,
		/* num_instrsp=*/ 0,
		/*xlate_reg_instr2** = */  0);

	if(retstatus == XLATE_TB_STATUS_NO_ERROR) {
		*rule_register = regtab.rules[reg].dw_regnum;
		*rule_offset = regtab.rules[reg].dw_offset;
		*rule_is_offset = regtab.rules[reg].dw_offset_relevant;
	}
	return retstatus;
}
