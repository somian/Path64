//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_leaf_iter.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
// ====================================================================


#ifndef opt_leaf_iter_INCLUDED
#define opt_leaf_iter_INCLUDED "opt_leaf_iter.h"

#include "cxx_template.h"
#include "opt_htable.h"

template <class NODE_TYPE>
void Expand_onto_stack(STACK<NODE_TYPE *> &, NODE_TYPE *);

void Expand_onto_stack(STACK<CODEREP *> &s, CODEREP *const cr);

void Expand_onto_stack(STACK<WN *> &s, WN *const wn);

template <class NODE_TYPE>
class LEAF_ITER {
private:
  STACK<NODE_TYPE *>  s;
  NODE_TYPE          *initial_node;  // For rewinding to support First().
  NODE_TYPE          *last;
  const BOOL          _tracing;

public:
  LEAF_ITER(MEM_POOL *const pool, const BOOL tracing) :
    _tracing(tracing), s(pool)
      { }

  LEAF_ITER(NODE_TYPE *node, MEM_POOL *pool, BOOL tracing) :
    _tracing(tracing), s(pool)
      { Init(node); }

  ~LEAF_ITER(void) { }

  void Init(NODE_TYPE *node)
    { initial_node = node; last = NULL; }

  BOOL Is_Empty(void) const
    {
      // Can't use (Elements() == 0) as a test because silly loop
      // iteration macros use Is_Empty as a loop termination
      // condition, rather than check whether Next() returned
      // NULL. For this reason, Is_Empty should really be called
      // Was_Empty_Last_Time().
      return (last == NULL);
    }

  NODE_TYPE *Next(void)
    {
      if (s.Elements() > 0) {
	NODE_TYPE *cur = s.Pop();
	last = cur;
	return cur;
      }
      else {
	last = NULL;
	return NULL;
      }
    }

  NODE_TYPE *First(void) { s.Clear();
			   Expand_onto_stack(s, initial_node);
			   return Next();	}

  NODE_TYPE *Cur(void) const    { return last; }
};

#endif
