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


xlate_address.c

$Revision$
$Date$

*/
#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
   #pragma weak xlate_address_xtnd = _xlate_address_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
   #pragma weak xlate_address = _xlate_address
#endif
int
xlate_address(xlate_table_con tab,
	int isNewAddress,
        Elf64_Addr addr_in,
	Elf64_Addr *addr_out,
	xlate_block *range)
{

   int retstatus  = XLATE_TB_STATUS_NO_ERROR;
   if(tab->xc_valid_table != VALID_TABLE_MAGIC) {
        return XLATE_TB_STATUS_INVALID_TABLE;
   }


   if(isNewAddress) {
	if(addr_in < tab->xc_hdr.ich_new_addr_low ||
	   addr_in >= tab->xc_hdr.ich_new_addr_high) {
	  retstatus = XLATE_TB_STATUS_NO_SUCH_ADDR_IN_TABLE;
	} else {
	  retstatus = tab->xc_search_for_addr_new(tab,
		isNewAddress,
		addr_in, addr_out,
		range);
	}

	

   } else {
	if(addr_in < tab->xc_hdr.ich_old_addr_low ||
	   addr_in >= tab->xc_hdr.ich_old_addr_high) {
	  retstatus = XLATE_TB_STATUS_NO_SUCH_ADDR_IN_TABLE;
	} else {
	  retstatus = tab->xc_search_for_addr_old(tab,
                isNewAddress,
                addr_in, addr_out,
                range);
	}
   }
   if(retstatus == XLATE_TB_STATUS_NO_SUCH_ADDR_IN_TABLE) {
#ifdef DEBUG
printf("XLATE_TB_STATUS_NO_SUCH_ADDR_IN_TABLE fabricate range\n"); /* */
#endif
      *addr_out = addr_in;
      if(range) {
	range->xe_new_address = addr_in;
	range->xe_old_address = addr_in;
	range->xe_old_range = INSTRUCTION_SIZE;
	range->xe_new_range = INSTRUCTION_SIZE;
      }
      retstatus = XLATE_TB_STATUS_NO_ERROR;
   }
   return retstatus;
}
