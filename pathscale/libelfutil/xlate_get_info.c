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


xlate_get_info.c

$Revision$
$Date$

*/

#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_get_info_xtnd = _xlate_get_info_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_get_info = _xlate_get_info
#endif


void
_xlate_get_infov1(xlate_table_con tab,
    Elf64_Sxword *      dataMoved,
    Elf64_Addr *        startup_fwa,
    Elf64_Addr *        startup_lwa,
    Elf64_Xword *        number_of_ranges,
    int        *        old_text_exists,
    int        *        old_text_alloc)
{
  xlate_header_v1 *tabhdrp;
  tabhdrp = (xlate_header_v1 *)tab->xc_section_data;
  if(dataMoved) {
	*dataMoved = tabhdrp->hd_data_moved;
  }
  if(startup_fwa) {
	*startup_fwa = tabhdrp->hd_startup_fwa;
	if(tab->xc_is64bit) {
		*startup_fwa |= ((Elf64_Addr)tabhdrp->hd_upper32_bits_new)<<32;
	}
  }
  if(startup_lwa) {
	*startup_lwa = tabhdrp->hd_startup_lwa;
	if(tab->xc_is64bit) {
		*startup_lwa |= ((Elf64_Addr)tabhdrp->hd_upper32_bits_new)<<32;
	}
  }
  if(number_of_ranges) {
	*number_of_ranges = tabhdrp->hd_num_entries;
  }
  if(old_text_exists) {
	*old_text_exists = (int)(0xff & (int)tabhdrp->hd_old_text_exists);
  }
  if(old_text_alloc) {
	*old_text_alloc = (int)(0xff & (int)tabhdrp->hd_old_text_alloc);
  }
  return;
}
void
_xlate_get_infov2_32(xlate_table_con tab,
    Elf64_Sxword *      dataMoved,
    Elf64_Addr *        startup_fwa,
    Elf64_Addr *        startup_lwa,
    Elf64_Xword *        number_of_ranges,
    int        *        old_text_exists,
    int        *        old_text_alloc)
{
  xlate_header32_v2 *tabhdrp;
  tabhdrp = (xlate_header32_v2 *)tab->xc_section_data;
  if(dataMoved) {
	*dataMoved = tabhdrp->hd_data_moved;
  }
  if(startup_fwa) {
	*startup_fwa = tabhdrp->hd_startup_fwa;
  }
  if(startup_lwa) {
	*startup_lwa = tabhdrp->hd_startup_lwa;
  }
  if(number_of_ranges) {
	*number_of_ranges = tabhdrp->hd_num_entries;
  }
  if(old_text_exists) {
	*old_text_exists = 0xff & tabhdrp->hd_old_text_exists;
  }
  if(old_text_alloc) {
	*old_text_alloc = 0xff & tabhdrp->hd_old_text_alloc;
  }
  return;
}
void
_xlate_get_infov2_64(xlate_table_con tab,
    Elf64_Sxword *      dataMoved,
    Elf64_Addr *        startup_fwa,
    Elf64_Addr *        startup_lwa,
    Elf64_Xword *        number_of_ranges,
    int        *        old_text_exists,
    int        *        old_text_alloc)
{
  xlate_header64_v2 *tabhdrp;
  tabhdrp = (xlate_header64_v2 *)tab->xc_section_data;

  if(dataMoved) {
	*dataMoved = tabhdrp->hd_data_moved;
  }
  if(startup_fwa) {
	*startup_fwa = tabhdrp->hd_startup_fwa;
  }
  if(startup_lwa) {
	*startup_lwa = tabhdrp->hd_startup_lwa;
  }
  if(number_of_ranges) {
	*number_of_ranges = tabhdrp->hd_num_entries;
  }
  if(old_text_exists) {
	*old_text_exists = 0xff & tabhdrp->hd_old_text_exists;
  }
  if(old_text_alloc) {
	*old_text_alloc = 0xff & tabhdrp->hd_old_text_alloc;
  }
}


int
xlate_get_info(xlate_table_con tab,
    Elf64_Sxword *      dataMoved,
    Elf64_Addr *        new_low_addr,
    Elf64_Addr *        old_low_addr,
    Elf64_Addr *        new_high_addr,
    Elf64_Addr *        old_high_addr,
    Elf64_Addr *        startup_fwa,
    Elf64_Addr *        startup_lwa,
    Elf64_Xword *        number_of_ranges,
    int        *        old_text_exists,
    int        *        old_text_alloc,
    int         *       is64bit ,
    xlate_tablekind *   tablekind ,
    int         *       tableversion )

{
   int retstatus = XLATE_TB_STATUS_NO_ERROR;

   if (tab->xc_valid_table != VALID_TABLE_MAGIC) {
	return XLATE_TB_STATUS_INVALID_TABLE;
   }
  
   if(new_high_addr) {	
	*new_high_addr = tab->xc_hdr.ich_new_addr_high;
   }
   if(new_low_addr) {
	*new_low_addr = tab->xc_hdr.ich_new_addr_low;
   }
   if(old_high_addr) {	
	*old_high_addr = tab->xc_hdr.ich_old_addr_high;
   }
   if(old_low_addr) {
	*old_low_addr = tab->xc_hdr.ich_old_addr_low;
   }
   if(is64bit) {
	*is64bit = tab->xc_is64bit;
   }
   if(tablekind) {
	*tablekind = tab->xc_hdr.ich_tablekind;
   }
   if(tableversion) {
	*tableversion = tab->xc_hdr.ich_version;
   }

   tab->xc_get_info_func(tab,dataMoved,startup_fwa,startup_lwa,
		number_of_ranges,
		old_text_exists,old_text_alloc);

   return retstatus;
}
