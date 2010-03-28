/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
#include <stdio.h>
#if defined(BUILD_OS_DARWIN)
#include <darwin_elf.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <sys/elf_whirl.h>
#include <sys/types.h>
#include <ctype.h>
#include <limits.h>
#include "wn.h"
#include "wn_map.h"
#include "wn_util.h"
#include "dwarf_DST.h"
#include "ipc_file.h"
#include "wb_util.h"
#include "cg_browser.h"
#include "cgb.h"
#include "cgb_ipa.h"
#include "opt_du.h"
#include "opt_alias_interface.h"
#include "dep_graph.h"
#include "prompf.h"
#include "wb.h"

CG_BROWSER cgb_ipa; 

extern void CGB_IPA_Initialize(IPA_CALL_GRAPH* ipa_cg)
{ 
  CGB_Set_Phase(CGBP_IPA); 
  WB_Set_Phase(WBP_IPA);
  CGB_Initialize(&cgb_ipa, ipa_cg); 
} 

extern void CGB_IPA_Terminate()
{ 
  CGB_Set_Phase(CGBP_NONE); 
  WB_Set_Phase(WBP_NONE);
  CGB_Terminate(&cgb_ipa); 
} 

extern void s_cg_ipa_debug(const char init_buffer[])
{ 
  cgb_ipa.Sdebug(init_buffer); 
} 

