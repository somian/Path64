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


#ifndef cgb_INCLUDED
#define cgb_INCLUDED

#ifndef cgb_browser_INCLUDED
#include "cg_browser.h"
#endif

enum CGB_PHASE {
  CGBP_NONE, 
  CGBP_IPA 
}; 

extern void CGB_Set_Phase(CGB_PHASE WB_Phase); 

extern void CGB_Initialize(CG_BROWSER* cgb, IPA_CALL_GRAPH* ipa_cg);

extern void CGB_Terminate(CG_BROWSER* cgb);

extern void cg_sdebug(const char init_buffer[]);

extern void scgb(const char init_buffer[]);
extern void cg_debug();

extern void cgb();

#endif /* cgb_INCLUDED */

