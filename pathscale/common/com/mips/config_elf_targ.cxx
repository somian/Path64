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


/* ====================================================================
 * ====================================================================
 *
 * Module: config_elf_targ.cxx
 * $Revision: 1.5 $
 * $Date: 05/05/09 13:00:37-07:00 $
 * $Author: gautam@jacinth.keyresearch $
 * $Source: common/com/mips/SCCS/s.config_elf_targ.cxx $
 *
 * Description:
 *
 * ELF configuration specific to the target machine/system.
 *
 * ====================================================================
 * ====================================================================
 */

#include "elf_stuff.h"
#include <sys/elf_whirl.h>
#include "defs.h"
#include "erglob.h"
#include "tracing.h"
#include "config_elf_targ.h"

/* ====================================================================
 *
 * Config_Target_From_ELF
 *
 * Based on the flags in the ELF header <ehdr>,
 * return whether is 64bit and the isa level.
 *
 * ====================================================================
 */
void Config_Target_From_ELF (Elf64_Word e_flags, BOOL *is_64bit, INT *isa)
{
  *is_64bit = (e_flags & EF_IRIX_ABI64);

  *isa = 1;
}


/* ====================================================================
 *
 * Config_ELF_From_Target
 *
 * Return the ELF specific flags for the current target ABI and ISA subset.
 *
 * ====================================================================
 */
Elf32_Word Config_ELF_From_Target(BOOL is_64bit, BOOL old_abi, INT isa)
{
  Elf32_Word e_flags = 0;

  if (is_64bit) e_flags |= EF_IRIX_ABI64;

  return e_flags;
}

Elf32_Half Get_Elf_Target_Machine (void)
{
// From /usr/include/elf.h
#define EM_MIPS_RS3_LE 10 /* MIPS R3000 little-endian */
  return EM_MIPS_RS3_LE;
}

