/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
#include <sys/types.h>
#if defined(BUILD_OS_DARWIN)
#include "darwin_elf.h"
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <ctype.h>
#include "wn.h"
#include "wn_map.h"
#include "wn_util.h"
#include <stdio.h>
#include "opt_du.h"
#include "opt_alias_mgr.h"
#include "dep_graph.h"
#include "cxx_template.h"
#include "prompf.h"
#include "ir_reader.h"
#include "wb_util.h"
#include "wb_buffer.h"
#include "wb_carray.h"
#include "wb_browser.h"
#include "wb.h"
#include "wb_f90_lower.h"

WB_BROWSER wb_f90_lower; 

extern void WB_F90_Lower_Initialize(WN* wn_global, 
			      WN_MAP Prompf_Id_Map)
{ 
  WB_Set_Phase(WBP_F90_LOWER); 
  WB_Initialize(&wb_f90_lower, wn_global, &Get_Current_PU(), 
    NULL, NULL, Prompf_Id_Map); 
} 

extern void WB_F90_Lower_Set_Parent_Map(WN_MAP Parent_Map)
{
  wb_f90_lower.Set_Parent_Map(Parent_Map); 
} 

extern void WB_F90_Lower_Set_Prompf_Info(PROMPF_INFO* prompf_info)
{
  wb_f90_lower.Set_Prompf_Info(prompf_info); 
} 

extern void WB_F90_Lower_Terminate()
{ 
  WB_Set_Phase(WBP_NONE); 
  WB_Terminate(&wb_f90_lower); 
} 

extern void s_f90_lower_debug(const char init_buffer[])
{ 
  wb_f90_lower.Sdebug(init_buffer); 
} 

