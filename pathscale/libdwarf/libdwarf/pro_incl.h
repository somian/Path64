/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000,2002,2004 Silicon Graphics, Inc.  All Rights Reserved.

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


#ifdef HAVE_ELF_H
#include <elf.h>
#elif defined(HAVE_LIBELF_H) 
/* On one platform without elf.h this gets Elf32_Rel 
   type defined (a required type). */
#include <libelf.h>
#endif
#if defined(BUILD_OS_DARWIN)
#include "darwin_elf.h"
#include <mach-o/reloc.h>
#endif /* defined(BUILD_OS_DARWIN) */

/* The target address is given: the place in the source integer
   is to be determined.
*/
#ifdef WORDS_BIGENDIAN
#define WRITE_UNALIGNED(dbg,dest,source, srclength,len_out) \
    { \
      dbg->de_copy_word(dest, \
                        ((char *)source) +srclength-len_out,  \
			len_out) ; \
    }


#else /* LITTLE ENDIAN */

#define WRITE_UNALIGNED(dbg,dest,source, srclength,len_out) \
    { \
      dbg->de_copy_word( (dest) , \
                        ((char *)source) ,  \
			len_out) ; \
    }
#endif



#include "libdwarf.h"

#include "dwarf.h"
#include "pro_opaque.h"
#include "pro_error.h"
#include "pro_util.h"
#include "pro_encode_nm.h"
#include "pro_alloc.h"
