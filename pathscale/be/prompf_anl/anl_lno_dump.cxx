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


#include <elf.h>                    /* for wn.h */
#include "wn.h"                     /* for WN */
#include "defs.h"		    /* for pu_info.h */ 
#include "config.h"                 /* for LNO_Path, etc. */
#include "pu_info.h"		    /* for PU_Info */ 
#include "w2c_driver.h"             /* for W2C_Process_Command_Line, etc. */
#include "w2f_driver.h"             /* for W2F_Process_Command_Line, etc. */
#include "anl_driver.h"             /* for Anl_Process_Command_Line, etc. */

extern WN_MAP Prompf_Id_Map; /* Maps WN constructs to unique identifiers */

#pragma weak W2C_Should_Emit_Nested_PUs
#pragma weak W2C_Outfile_Translate_Pu
#pragma weak W2F_Should_Emit_Nested_PUs
#pragma weak W2F_Outfile_Translate_Pu


extern void Prompf_Emit_Whirl_to_Source(PU_Info* current_pu,
                                        WN* func_nd)
{
  ST_IDX   st = PU_Info_proc_sym(current_pu);

  BOOL nested = ST_level(&St_Table[st]) > 2;

  if (Anl_Needs_Whirl2c() && 
      (W2C_Should_Emit_Nested_PUs() || !nested))
    W2C_Outfile_Translate_Pu(func_nd, TRUE);
  else if (Anl_Needs_Whirl2f() &&  
	   (W2F_Should_Emit_Nested_PUs() || !nested))
    W2F_Outfile_Translate_Pu(func_nd);
}


