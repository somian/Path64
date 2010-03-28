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


// This code is really -*- c++ -*-

#ifndef CXX_QUEUE_INCLUDED
#define CXX_QUEUE_INCLUDED "cxx_queue.h"

template <class ITEM_TYPE>
class QUEUE_NODE {
private:
  ITEM_TYPE   _queue_item;
  QUEUE_NODE *_queue_next;

public:
  ITEM_TYPE Qnode_Item() {
    return _queue_item;
  }
  QUEUE_NODE *Qnode_Next() {
    return _queue_next;
  }
  void Qnode_Next(QUEUE_NODE *n) {
    _queue_next = n;
  }
  QUEUE_NODE(ITEM_TYPE item) {
    _queue_item = item;
    _queue_next = NULL;
  }
};

template<class ITEM_TYPE>
class QUEUE {
private:
  MEM_POOL *_pool;
  UINT32 _length;
  QUEUE_NODE<ITEM_TYPE> *_first;
  QUEUE_NODE<ITEM_TYPE> *_last;
public:
  QUEUE_NODE<ITEM_TYPE> *Queue_First() const{
    return _first;
  }
  QUEUE_NODE<ITEM_TYPE> *Queue_Last() const{
    return _last;
  }
  UINT32 Queue_Length() const{
    return _length;
  }
  BOOL Queue_Isempty() {
    return (0 == _length);
  }
  // A constructor
  QUEUE<ITEM_TYPE> (MEM_POOL *pool);
  void      Add_Tail_Q (ITEM_TYPE);
  ITEM_TYPE Get_Q ();
  ITEM_TYPE Get_Tail_Q();
  INT32     Index(ITEM_TYPE item, BOOL Insert_If_Absent=FALSE);
};

template <class ITEM_TYPE>
class QUEUE_ITER {
private:
  QUEUE_NODE<ITEM_TYPE> *_node;
  const QUEUE<ITEM_TYPE> *_q;
public:
  BOOL        Step (ITEM_TYPE *i) {
    if (NULL == _node) 
      return FALSE;
    else {
      *i = _node->Qnode_Item();
      _node = _node->Qnode_Next();
      return TRUE;
    }
  }
  QUEUE_ITER       (const QUEUE<ITEM_TYPE> *q) : _q (q) {
    _node = q->Queue_First();
  }
};

/* A wklist q is different from an ordinary q in that it is mutable.
   Essentially, it exports the queue underneath it, so that an outer
   level of the iterator can modify it. */

template<class ITEM_TYPE>
class QUEUE_WKLIST_ITER {
private:
  QUEUE_NODE<ITEM_TYPE> *_node;
  QUEUE<ITEM_TYPE>      *_q;
  MEM_POOL              *_pool;
public:
  QUEUE<ITEM_TYPE> *Wklist_Queue() {
    return _q;
  }
  BOOL Step(ITEM_TYPE *i) {
    assert (NULL != _q);
    if (0 == _q->Queue_Length()) 
      return FALSE;
    else {
      *i = _q->Get_Q();
      return TRUE;
    }
  }
  QUEUE_WKLIST_ITER (const ITEM_TYPE i, MEM_POOL *pool) {
    _pool = pool;
    _q = CXX_NEW (QUEUE<ITEM_TYPE>(pool), pool);
    _q->Add_Tail_Q (i);
  }
};

#ifdef __GNUC__
// Implementation stuff included here because g++
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

#endif
