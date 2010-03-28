/*
 *  Copyright (C) 2007 QLogic Corporation.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005 PathScale, Inc.  All Rights Reserved.
 */

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


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "elf_stuff.h"
#include "sections.h"

#define INST_BYTES 16

SECTION Sections[_SEC_INDEX_MAX] = {
  {_SEC_UNKNOWN,NULL,
     0,
	0, 0, 
     0, ".unknown", 0},
  {_SEC_TEXT,	NULL,
     0|SHF_EXECINSTR|SHF_ALLOC,
	SHT_PROGBITS, INST_BYTES, 
     INT64_MAX, ELF_TEXT, 0},
  {_SEC_DATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC, 
	SHT_PROGBITS, 0, 
     INT64_MAX, ELF_DATA, 0},
  {_SEC_SDATA,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     INT32_MAX, MIPS_SDATA, 0},
#ifdef KEY
  // Create a new section _SEC_LDATA_MIPS_LOCAL which is the same as the
  // non-KEY _SEC_LDATA so that we can allocate OpenMP thread-private symbols
  // to it, in order to preserve the old OpenMP thread-private behavior.  This
  // frees up _SEC_LDATA to mean ELF_TDATA for thread-local storage (TLS).
  // Bug 12619.
  {_SEC_LDATA_MIPS_LOCAL,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_PROGBITS, 0, 
     INT64_MAX, ".MIPS.ldata", 0},
  {_SEC_LDATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_TLS,
	SHT_PROGBITS, 0, 
     INT64_MAX, ELF_TDATA, 0},
#else
  {_SEC_LDATA,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_PROGBITS, 0, 
     INT64_MAX, ".MIPS.ldata", 0},
#endif
  {_SEC_RDATA,	NULL,
     0|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     INT64_MAX, ELF_RODATA, 0},
  {_SEC_SRDATA,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     INT32_MAX, MIPS_SRDATA, 0},
  {_SEC_LIT4,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 4, 
     INT32_MAX, MIPS_LIT4, 0},
  {_SEC_LIT8,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 8, 
     INT32_MAX, MIPS_LIT8, 0},
  {_SEC_LIT16,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC|SHF_MIPS_MERGE,
	SHT_PROGBITS, 16, 
     INT32_MAX, MIPS_LIT16, 0},
  {_SEC_BSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC,
	SHT_NOBITS, 0, 
     INT64_MAX, ELF_BSS, 0},
  {_SEC_SBSS,	NULL,
     0|SHF_WRITE|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_NOBITS, 0, 
     INT32_MAX, MIPS_SBSS, 0},
#ifndef linux
  {_SEC_LBSS,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_LOCAL,
	SHT_NOBITS, 0, 
     INT64_MAX, MIPS_LBSS, 0},
#else
  // There is no MIPS_LBSS section on Linux, but we need a space holder
  {_SEC_LBSS,   NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif
  {_SEC_GOT,	NULL,
     0|SHF_IA_64_SHORT|SHF_ALLOC,
	SHT_PROGBITS, 0, 
     INT32_MAX, ELF_GOT, 0},
  {_SEC_CPLINIT,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, "__cplinit", 0},
#ifndef linux
  {_SEC_EH_REGION,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION, 0},
  {_SEC_EH_REGION_SUPP,	NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0, 
     INT64_MAX, MIPS_EH_REGION_SUPP, 0},
#else
#ifdef KEY
  {_SEC_EH_REGION,      NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
        SHT_PROGBITS, 0,
     INT64_MAX, ".except_table", 0},
  {_SEC_EH_REGION_SUPP, NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
        SHT_PROGBITS, 0,
     INT64_MAX, ".except_table_supp", 0},
#else
  // It's not yet clear what to do about the EH_REGION sections on Linux
  {_SEC_EH_REGION,      NULL,
     0,
        0, 0,
     0, ".unknown", 0},
  {_SEC_EH_REGION_SUPP, NULL,
     0,
        0, 0,
     0, ".unknown", 0},
#endif // KEY
#endif
  {_SEC_DISTR_ARRAY,  NULL,
     0|SHF_WRITE|SHF_ALLOC|SHF_MIPS_NAMES,
	SHT_PROGBITS, 0,
     INT64_MAX, "_MIPS_distr_array", 0},
  {_SEC_DATA_REL_RO,  NULL,
     0|SHF_WRITE|SHF_ALLOC,
        SHT_PROGBITS, 0,
     INT64_MAX, ".data.rel.ro.local", 0},
};

extern SECTION_IDX
Corresponding_Short_Section (SECTION_IDX sec)
{
   switch ( sec ) {
   case _SEC_DATA:      return _SEC_SDATA;
   case _SEC_RDATA:     return _SEC_SRDATA;
   case _SEC_BSS:       return _SEC_SBSS;
   default:		return sec;
   }
}

extern BOOL
SEC_is_gprel (SECTION_IDX sec)
{
	return (SEC_flags(sec) & SHF_IA_64_SHORT);
}

extern BOOL
SEC_is_merge (SECTION_IDX sec)
{
	return (SEC_flags(sec) & SHF_MIPS_MERGE);
}

extern BOOL
SEC_is_exec (SECTION_IDX sec)
{
	return (SEC_flags(sec) & SHF_EXECINSTR);
}

extern BOOL
SEC_is_nobits (SECTION_IDX sec)
{
	return (SEC_type(sec) & SHT_NOBITS);
}

