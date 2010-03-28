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

#ifndef ipo_clone_INCLUDED
#define ipo_clone_INCLUDED

// to avoid extra includes; 
struct ip_file_hdr;

#ifndef cxx_base_INCLUDED
#include "cxx_base.h"
#endif


//-------------------------------------------------------------------
// use an array of such lists to store st entries that have already
// been copied during the cloning process
//-------------------------------------------------------------------
class CLONED_NODE: public SLIST_NODE {
private:
  ST* _st, *_copy_st;
public:
  DECLARE_SLIST_NODE_CLASS(CLONED_NODE);
  void Set_orig_st(ST* s) { _st = s;};
  void Set_copy_st(ST* s) { _copy_st = s;};
  ST* Get_orig_st() { return _st;};
  ST* Get_copy_st() { return _copy_st;};

  CLONED_NODE(ST* orig, ST* copy);
};

class CLONED_LIST: public SLIST {
public:
  DECLARE_SLIST_CLASS(CLONED_LIST, CLONED_NODE);
  void Append(ST*,ST*, MEM_POOL *m);
  ST* Lookup(ST* orig_st); 
};


class CLONED_LIST_ITER: public SLIST_ITER {

public:
  DECLARE_SLIST_ITER_CLASS(CLONED_LIST_ITER,CLONED_NODE,CLONED_LIST);
};

// Main entry points to cloning

extern void IPO_Clone (IPA_NODE* orig_node, IPA_NODE* clone_node);

extern WN* IPO_Copy_Tree (WN *);
 
#if (!defined(_STANDALONE_INLINER) && !defined(_LIGHTWEIGHT_INLINER))

/* this routine is responsible for updating the st entry in the call
   to reflect the call to the clone. */
extern void Clone_update_st(WN *, ST_IDX, SYMTAB_IDX, MEM_POOL*,
                            IPA_NODE *);
#endif
#endif /* ipo_clone_INCLUDED */
