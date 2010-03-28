/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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



/* Define to 1 if the elf64_getshdr function is in libelf.a */
#undef HAVE_ELF64_GETSHDR

/* Define to 1 if the elf64_getehdr function is in libelf.a */
#undef HAVE_ELF64_GETEHDR


/* see if __uint32_t is predefined in the compiler */
#undef HAVE___UINT32_T

/* see if __uint64_t is predefined in the compiler */
#undef HAVE___UINT64_T

/* Define 1 if sys/types.h defines __uint32_t */
#undef HAVE___UINT32_T_IN_SYS_TYPES_H

/* Define 1 if  R_IA_64_DIR32LSB is defined (might be enum value) */
#undef HAVE_R_IA_64_DIR32LSB

/* Define 1 if sys/ia64/elf.h exists*/
#undef HAVE_SYS_IA64_ELF_H

/* Define 1 if want to build with 32/64bit section offsets for ia64 */
/* per the dwarf2 committee proposal adopted Dec 1999 */
#undef HAVE_DWARF2_99_EXTENSION

/* Define 1 if want only 32bit section offsets per pure dwarf2.0.0 spec */
/* Only one of HAVE_OLD_DWARF2_32BIT_OFFSET or HAVE_DWARF2_99_EXTENSION */
/* may be defined */
#undef HAVE_OLD_DWARF2_32BIT_OFFSET

