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


#ifndef config_elf_targ_INCLUDED
#define config_elf_targ_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ====================================================================
 *
 * Module: config_elf_targ.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * ELF configuration specific to the target machine/system.
 *
 * ====================================================================
 * ====================================================================
 */


extern void
Config_Target_From_ELF (Elf64_Word e_flags, BOOL *is_64bit, INT *isa);
extern Elf32_Word Config_ELF_From_Target (BOOL is_64bit, BOOL old_abi, INT isa);
extern Elf32_Half Get_Elf_Target_Machine (void);

#ifdef __cplusplus
}
#endif
#endif /* config_elf_targ_INCLUDED */
