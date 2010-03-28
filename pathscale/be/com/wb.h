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


#ifndef wb_INCLUDED
#define wb_INCLUDED

#ifndef wb_browser_INCLUDED
#include "wb_browser.h"
#endif

enum WB_PHASE {
  WBP_NONE, 
  WBP_F90_LOWER, 
  WBP_PROMPF_SA, 
  WBP_OMP_PRELOWER, 
  WBP_LNO, 
  WBP_LOWER,
  WBP_IPL,
  WBP_IPA
}; 

extern WB_PHASE WB_Phase(); 

extern void WB_Set_Phase(WB_PHASE WB_Phase); 

extern void WB_Initialize(WB_BROWSER* wb, WN* wn_global, PU* pu, 
  DU_MANAGER* du, ALIAS_MANAGER* alias_mgr, WN_MAP Prompf_Id_Map);

extern void WB_Terminate(WB_BROWSER* wb);

extern void sdebug(const char init_buffer[]);

extern void swb(char init_buffer[]);

extern void debug();

extern void wb();

extern void debug_root();

extern void wb_root(WN* root);

#endif /* wb_INCLUDED */
