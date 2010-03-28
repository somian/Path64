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



/*
  This is a uniform set of base includes for the xlate code
  This is the #include in the c files.
  xlateincl.h

  $Revision: 1.5 $
  $Date: 05/05/25 10:37:42-07:00 $

*/

#ifndef _LP64
#include <sgidefs.h>
#endif /* _LP64 */
#include <stdio.h> /* for debug printf and for NULL decl */
#include "xlatebase.h"
#include "syn.h"
#if defined(BUILD_OS_DARWIN)
#include "darwin_elf.h"
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include "libelf/libelf.h"
#include "dwarf_stuff.h"
#include "libXlate.h"
#include <cmplrs/xlate.h>
#include <cmplrs/leb128.h>
#include <bstring.h> /* for bzero */
#include <string.h> /* for memcpy */
#include <stdlib.h>
#include "xlateTypes.h"
#include "xlate_proTypes.h"
#include "xlatexterns.h"
#include "xlate_pro_externs.h"
