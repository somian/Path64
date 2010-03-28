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

This declares basic types used inside the xlate code.
The  types are sized for the size of compilation: that is
in 64-bit compile they are 64 bit.

xlatebase.h

$Revision$
$Date$

*/

/*
The following go with the size of the address space that
can be used given how the library is compiled.
*/
#if _MIPS_SIM == _MIPS_SIM_ABI64

typedef __uint64_t Uword;
typedef __int64_t  Sword;

#else
typedef __uint32_t Uword;
typedef __int32_t  Sword;

#endif

/* 
   The following are 64 bits always , since we
   process things with 64bit addrs even when the lib
   compiled 32 bits. Though we cannot have more than
   a fraction of these numbers of things actually]
   present if compiling the library 32 bit.
*/

typedef __uint64_t Xuword;
typedef __int64_t  Xsword;

#define INSTRUCTION_SIZE 4

#undef _LIBELF_XTND_EXPANDED_DATA /* libelf internal #define */
#ifdef _LIBELF_XTND_64            /* public documented #define */
#if  (_MIPS_SZLONG == 32)
/* When building for a 32bit abi, use extended definition if requested */
#   define _LIBELF_XTND_EXPANDED_DATA   1
#endif
#endif


/*
The following prevent accidental calls from xtdn lib to non-xtnd
and vice versa.
The list is intended to be of every global except those in
the public interface.
*/
#ifdef _LIBELF_XTND_EXPANDED_DATA
#define _xlate_init_fd           _xlate_init_fd_xtnd
#define _xlate_init_elf          _xlate_init_elf_xtnd
#define _xlate_get_info          _xlate_get_info_xtnd
#define _xlate_address           _xlate_address_xtnd
#define _xlate_get_reg_rule      _xlate_get_reg_rule_xtnd
#define _xlate_get_all_reg_rules _xlate_get_all_reg_rules_xtnd
#define _xlate_expand_reg_info   _xlate_expand_reg_info_xtnd
#define _xlate_expand_reg_info2  _xlate_expand_reg_info2_xtnd
#define _xlate_finish            _xlate_finish_xtnd

#define _xlate_pro_init          _xlate_pro_init_xtnd
#define _xlate_pro_add_info      _xlate_pro_add_info_xtnd
#define _xlate_pro_add_reg_info  _xlate_pro_add_reg_info_xtnd
#define _xlate_pro_add_range     _xlate_pro_add_range_xtnd
#define _xlate_pro_disk_header   _xlate_pro_disk_header_xtnd
#define _xlate_pro_disk_next_block _xlate_pro_disk_next_block_xtnd
#define _xlate_pro_finish        _xlate_pro_finish_xtnd
#endif
