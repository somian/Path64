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
   xlate_pro_add_info.c
   $Revision$


*/

#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_pro_add_info_xtnd = _xlate_pro_add_info_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_pro_add_info = _xlate_pro_add_info
#endif

int xlate_pro_add_info(xlate_table_pro     table,
    Elf64_Sxword         data_moved,
    Elf64_Addr          startup_fwa,
    Elf64_Addr          startup_lwa,
    Elf32_Word          old_text_exists,
    Elf32_Word          old_text_alloc)

{
   int retstatus  = XLATE_TB_STATUS_NO_ERROR;
   if(table->tb_magic != PRO_MAGIC_VAL) {
        return XLATE_TB_STATUS_INVALID_TABLE;
   }
   if(table->tb_no_new_entries_allowed ) {
	return XLATE_TB_STATUS_ADD_TOO_LATE;
   }
        /* from xlate_pro_add_info */
   table->tb_data_moved = data_moved;
   table->tb_startup_fwa = startup_fwa;
   table->tb_startup_lwa = startup_lwa;
   table->tb_old_text_exists = (unsigned char)old_text_exists;
   table->tb_old_text_alloc = (unsigned char)old_text_alloc;

   return retstatus;
}


