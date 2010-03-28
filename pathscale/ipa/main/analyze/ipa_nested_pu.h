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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef cxx_ipa_nested_pu_INCLUDED
#define cxx_ipa_nested_pu_INCLUDED

#ifndef cxx_ipa_cg_INCLUDED
#include "ipa_cg.h"			// CXX_NEW, etc.
#endif /* cxx_ipa_cg_INCLUDED */

extern IPA_NODE* 
Get_Parent_Of_Nested_PU(IPA_NODE* child);

extern  void 
Build_Nested_Pu_Relations();

#endif //  cxx_ipa_nested_pu_INCLUDED
