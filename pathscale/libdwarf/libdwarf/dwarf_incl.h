/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2002, 2004 Silicon Graphics, Inc.  All Rights Reserved.

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



#ifndef DWARF_INCL_H
#define DWARF_INCL_H

#include "libdwarfdefs.h"
#include <string.h>

#if defined(BUILD_OS_DARWIN)
#include "darwin_elf.h"
#else /* defined(BUILD_OS_DARWIN) */
#ifdef HAVE_ELF_H
#include <elf.h>
#endif
#endif /* defined(BUILD_OS_DARWIN) */

#include <limits.h>
#include "dwarf_stuff.h"

#include "dwarf_base_types.h"
#include "dwarf_alloc.h"
#include "dwarf_opaque.h"
#include "dwarf_error.h"
#include "dwarf_util.h"
#endif /* DWARF_INCL_H */
