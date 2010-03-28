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


#ifndef __SYS_ELF_WHIRL_H__
#define __SYS_ELF_WHIRL_H__

/* $Header$ */

/* Elf extension for WHIRL object files */

/* Revision Number: Should be incremented whenever INCOMPATIBLE changes are
   made.  This string is put in the ".comment" section.  Refer to ELF
   object file spec. for format */
#if defined(TARG_IA64) && !defined(__ia64)
#define WHIRL_REVISION	"WHIRL::0.33:IA64X"
#else
#define WHIRL_REVISION	"WHIRL::0.33:"
#endif

/*
 * sh_info field for section of type SHT_MIPS_WHIRL 
 */

#define WT_NULL		0x0
#define WT_PU_SECTION	0x1	    /* all PU-specific information */
#define WT_GLOBALS	0x2	    /* WHIRL global symbol table */
#define WT_COMP_FLAGS	0x3	    /* compilation flags for this object */
#define WT_STRTAB	0x4	    /* WHIRL string table */
#define WT_CONSTAB	0x5	    /* WHIRL constant table */
#define WT_IPA_SUMMARY	0x6	    /* IPA summary information */
#define WT_DST		0x7	    /* WHIRL Debug Symbol Table */
#define WT_LOCALMAP	0x8	    /* IPAA local map */

/*
 * Special WHIRL section names.
 */
#define MIPS_WHIRL_PU_SECTION	".WHIRL.pu_section"
#define MIPS_WHIRL_GLOBALS	".WHIRL.globals"
#define MIPS_WHIRL_COMP_FLAGS	".WHIRL.flags"
#define MIPS_WHIRL_STRTAB	".WHIRL.strtab"
#define MIPS_WHIRL_CONSTAB	".WHIRL.constab"
#define MIPS_WHIRL_SUMMARY	".WHIRL.summary"
#define MIPS_WHIRL_DST		".WHIRL.dst"
#define MIPS_WHIRL_LOCALMAP	".WHIRL.localmap"

#endif /* __SYS_ELF_WHIRL_H__ */
