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


#ifndef ipa_reshape_INCLUDED
#define ipa_reshape_INCLUDED "ipa_reshape.h"

extern PROJECTED_REGION* Projected_Region_To_Memory(IPA_NODE* node,
			                            PROJECTED_REGION* pr,
			                            MEM_POOL* mem_pool);

extern BOOL Try_Reshape_Callee_Formal(IPA_NODE* ipan_caller,
			              IPA_NODE* ipan_callee,
			              SUMMARY_CALLSITE* sk, 
			              INT position,
			              MEM_POOL* mem_pool);

extern BOOL Mismatched_Types(IPA_NODE* ipan_caller, IPA_NODE*
		             ipan_callee,
		             SUMMARY_CALLSITE *sk,
		             INT position,
		             MEM_POOL *mem_pool);

#endif /* ipa_reshape_INCLUDED */

