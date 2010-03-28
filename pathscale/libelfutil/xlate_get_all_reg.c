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

  xlate_get_all_reg.c

  $Revision$


*/

#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_get_all_reg_rules_xtnd = _xlate_get_all_reg_rules_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_get_all_reg_rules = _xlate_get_all_reg_rules
#endif

int
xlate_get_all_reg_rules(xlate_table_con tab,
        Elf64_Addr pc,
	Dwarf_Regtable *reg_table)
{
	int retstatus;
	if(tab->xc_valid_table != VALID_TABLE_MAGIC) {
        	return XLATE_TB_STATUS_INVALID_TABLE;
   	}



        retstatus = _xlate_expand_reg_info_internal(tab,
                ALL_REG_RULES,
                pc,
                reg_table,
                /* num_instrsp=*/ 0,
		/*xlate_reg_instr2*= */  0);

	return retstatus;
}


