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
#pragma weak xlate_expand_reg_info2_xtnd = _xlate_expand_reg_info2_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_expand_reg_info2 = _xlate_expand_reg_info2
#endif

/* preferred version */
int
xlate_expand_reg_info2(xlate_table_con tab,
	Elf64_Xword * num_instrs,
	xlate_reg_instr2 ** instructions)
{
        Dwarf_Regtable regtab;
	int retstatus;

        retstatus = _xlate_expand_reg_info_internal(tab,
                ALL_INSTRUCTIONS,
                /* pc */0,
                &regtab,
                num_instrs,
		instructions);

	return retstatus;
}

