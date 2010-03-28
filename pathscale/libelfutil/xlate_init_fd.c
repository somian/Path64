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

Given an fd, open up an xlate consumer handle

xlate_init_fd.c

$Revision$
$Date$

*/
#include "xlateincl.h"

#ifdef _LIBELF_XTND_EXPANDED_DATA
#pragma weak xlate_init_fd_xtnd  = _xlate_init_fd_xtnd
#elif defined(BUILD_OS_DARWIN)
#else
#pragma weak xlate_init_fd  = _xlate_init_fd
#endif

int
xlate_init_fd(int fd, int open_debug_table,
	xlate_table_con * ret_tab_ptr)
{

   Elf *elf;
   xlate_table_con ltab = 0;
   int retstatus  = XLATE_TB_STATUS_NO_ERROR;

   if(elf_version(EV_CURRENT) == EV_NONE) {
	return XLATE_TB_STATUS_ELF_VERSION_BAD;
   }

#if defined(linux) || defined(BUILD_OS_DARWIN)
   elf = elf_begin(fd,ELF_C_READ,NULL);
#else
   elf = elf_begin(fd,ELF_C_READ_MMAP,NULL);
#endif
   if(elf == NULL) {
	return XLATE_TB_STATUS_ELF_BEGIN_BAD;
   }

   retstatus = xlate_init_elf(elf,
		open_debug_table,&ltab);

   if(retstatus != XLATE_TB_STATUS_NO_ERROR) {
       (void)elf_end(elf);
   } else {
	ltab->xc_did_elf_begin = 1;
	*ret_tab_ptr = ltab;
   }

   return retstatus;
}
