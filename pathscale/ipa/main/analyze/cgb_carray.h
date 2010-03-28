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


#ifndef cgb_carray_INCLUDED
#define cgb_carray_INCLUDED "cgb_carray.h"

#ifndef dwarf_DST_INCLUDED
#include "dwarf_DST.h"
#endif

#ifndef ipc_file_INCLUDED
#include "ipc_file.h"
#endif

#ifndef cxx_ipa_cg_INCLUDED
#include "ipa_cg.h"
#endif 

const INT CGB_MAX_SAVED_NODES = 500;

class CGB_CARRAY {
  INT _next_index; 
  IPA_NODE* _nodes[CGB_MAX_SAVED_NODES];
  NODE_INDEX _vertices[CGB_MAX_SAVED_NODES];
public: 
  void Reset_Index() { _next_index = 0; }
  INT Next_Index() { return _next_index; }
  IPA_NODE* Node(INT element) { return _nodes[element]; }
  NODE_INDEX Vertex(INT element) { return _vertices[element]; }
  void Enter_This_Pair(IPA_NODE* ipan, NODE_INDEX v);
  INT Enter_This_Pair_Unique(IPA_NODE* ipan, NODE_INDEX v);
  INT Find_This_Pair(IPA_NODE* ipan, NODE_INDEX v);
  void List_All_Pairs(FILE* fp);
};

#endif /* cgb_carray_INCLUDED */
