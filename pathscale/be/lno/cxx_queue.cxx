/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#include <sys/types.h>
#include <stdio.h>
#include "cxx_memory.h"
#include "errors.h"
#include "cxx_queue.h"

#ifndef __GNUC__
// Implementation stuff excluded here because g++
// (rightly) doesn't do implicit .cxx file inclusion.

template <class ITEM_TYPE>
QUEUE<ITEM_TYPE>::QUEUE (MEM_POOL *pool)
{
  _pool = pool;
  _length = 0;
  _first = _last = NULL;
}

template <class ITEM_TYPE>
void
QUEUE<ITEM_TYPE>::Add_Tail_Q(ITEM_TYPE item)
{
  QUEUE_NODE<ITEM_TYPE> *node;

  node = CXX_NEW (QUEUE_NODE<ITEM_TYPE>(item), _pool);
  if (0 == _length) 
    _first = _last = node;
  else {
    _last->Qnode_Next(node);
    _last = node;
  }
  _length++;
  return;
}

template<class ITEM_TYPE>
ITEM_TYPE
QUEUE<ITEM_TYPE>::Get_Q ()
{
  ITEM_TYPE item;
  QUEUE_NODE<ITEM_TYPE> *node;

  if (0 == _length) 
    return (ITEM_TYPE) 0;
  node = _first;
  item = node->Qnode_Item();
  /* Remove node from queue */
  _first = node->Qnode_Next();
  _length--;
  if (0 == _length) 
    _last = NULL;
  return item;
}

template<class ITEM_TYPE>
ITEM_TYPE
QUEUE<ITEM_TYPE>::Get_Tail_Q ()
{
  ITEM_TYPE item;
  QUEUE_NODE<ITEM_TYPE> *node;
  INT32 count, i;

  if (0 == _length)
    return (ITEM_TYPE) 0;
  else if (1 == _length) 
    return this->Get_Q();
  else {
    count = _length - 2;
    node = _first;
    for (i = 0; i < count; i++) {
      node = node->Qnode_Next();
    }
    assert (node->Qnode_Next() == _last);
    item = _last->Qnode_Item();
    node->Qnode_Next(NULL);
    _length--;
    _last = node;
    return item;
  }
}

template<class ITEM_TYPE>
INT32
QUEUE<ITEM_TYPE>::Index(ITEM_TYPE item, BOOL Insert_If_Absent)
{
  INT32 ret_val = 0;
  QUEUE_NODE<ITEM_TYPE> *node = _first;
  while (NULL != node) {
    if (node->Qnode_Item() == item)
      return ret_val;
    node = node->Qnode_Next();
    ret_val++;
  }
  FmtAssert (ret_val == _length, ("Inconsistency in queue index function"));
  if (Insert_If_Absent) {
    this->Add_Tail_Q (item);
    return ret_val;
  }
  else
    return -1;
}

#endif 
