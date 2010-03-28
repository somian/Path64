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

Given an Elf *, open up an xlate consumer handle

xlate_init_elf.c

$Revision$
$Date$

*/
#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_init_elf_xtnd  = _xlate_init_elf_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_init_elf  = _xlate_init_elf
#endif

int
xlate_init_elf(Elf * elf, int open_debug_table,
	xlate_table_con * ret_tab_ptr)
{
   int res;
   Elf32_Word wanttype;
   Elf_Scn *scn = 0;
   int is64bit = 0;
   char *ident;
   size_t identsize;
   Elf64_Shdr *shdr64 = 0;
   Elf32_Shdr *shdr32 = 0;
   Elf32_Ehdr *ehdr32 = 0;
   Elf64_Ehdr *ehdr64 = 0;
   int stringindex = 0;
   char * sec_name;

   if(elf_kind(elf) != ELF_K_ELF) {
	return XLATE_TB_STATUS_NOT_ELF;
   }
   ident = elf_getident(elf,&identsize);

   if(open_debug_table == XLATE_OPEN_STD_TABLE) {
	/* we take the first SHT_MIPS_XLATE*/
	wanttype = SHT_MIPS_XLATE;
   } else if(open_debug_table == XLATE_OPEN_DEBUG_TABLE) {
	/* we take the first SHT_MIPS_XLATE_DEBUG */
	wanttype = SHT_MIPS_XLATE_DEBUG;
   } else {
	return XLATE_TB_STATUS_NO_XLATE;
   }
   res = XLATE_TB_STATUS_NO_XLATE;
   if(ident == 0 || identsize < EI_NIDENT) {
     return  XLATE_TB_STATUS_ELF_IDENT_BAD;
   }
   if(ident[EI_CLASS] ==  ELFCLASS64) {
	is64bit = 1;
   }
   if(is64bit) {
	ehdr64 = elf64_getehdr(elf);
	if(ehdr64 == 0 ) {
	  return XLATE_TB_STATUS_ELF_EHDR_BAD;
	}
	stringindex = ehdr64->e_shstrndx;
   } else {
	ehdr32 = elf32_getehdr(elf);
	if(ehdr32 == 0 ) {
	  return XLATE_TB_STATUS_ELF_EHDR_BAD;
	}
	stringindex = ehdr32->e_shstrndx;
   }

   for(scn = elf_nextscn(elf,scn);  
	scn != 0 
		; scn = elf_nextscn(elf,scn)) {
	if(is64bit) {
	  shdr64 = elf64_getshdr(scn);
	  if(shdr64 == 0) {
	    return XLATE_TB_STATUS_ELF_SHDR_BAD;
	  }
	  sec_name = elf_strptr(elf,stringindex,shdr64->sh_name);
	  if(sec_name == 0) {
	    return XLATE_TB_STATUS_ELF_STRPTR_BAD;
	  }
	  if(shdr64->sh_type != wanttype) {
		continue;
	  }
	  res = xlate_named_init_elf(elf,
		sec_name,ret_tab_ptr);
	  return res;
	} else {
	  shdr32 = elf32_getshdr(scn);
	  if(shdr32 == 0) {
	    return XLATE_TB_STATUS_ELF_SHDR_BAD;
	  }
	  sec_name = elf_strptr(elf,stringindex,shdr32->sh_name);
	  if(sec_name == 0) {
	    return XLATE_TB_STATUS_ELF_STRPTR_BAD;
	  }
	  if(shdr32->sh_type != wanttype) {
		continue;
	  }
	  res = xlate_named_init_elf(elf,
		sec_name,ret_tab_ptr);
	  return res;
	}
   }

   return res;
}
