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


//* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#ifndef ipa_section_main_INCLUDED
#define ipa_section_main_INCLUDED

#ifndef ipa_section_INCLUDED
#include "ipa_section.h"
#endif

extern INT 
Locate_symbol(LOOP_SYMBOL_ARRAY* syms, 
              SYSTEM_OF_EQUATIONS* soe, 
              const LOOP_SYMBOL& symbol);


extern void
Add_to_SOE(PROJECTED_REGION* a, 
           const INT pos, 
           SYSTEM_OF_EQUATIONS* soe,
	   const BOOL convert_equation, 
	   LOOP_SYMBOL_ARRAY* sym, 
           INT depth, 
           BOOL trace);

#ifndef IPA_SUMMARY
extern void Init_IPA_Print_Arrays (IPA_NODE* node);
extern INT IPA_Ivar_Count;
#endif

#endif 
