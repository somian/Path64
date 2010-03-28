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


#ifndef wb_util_INCLUDED
#define wb_util_INCLUDED "wb_util.h"

enum WB_SANITY_CHECK_LEVEL {
  WB_SC_DISABLE,
  WB_SC_DU_ONLY,
  WB_SC_DU_AND_ARRAY,
  WB_SC_FULL_SNL
};

extern void WB_Bell();
extern void WB_Prompt();
extern const char* WB_Whirl_Symbol(WN* wn); 
extern INT WB_Dump_Whirl_Expr(WN* wn_root, WN* wn, char* buffer, INT cc);

#endif /* wb_util_INCLUDED */
