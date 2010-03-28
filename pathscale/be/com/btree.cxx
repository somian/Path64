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


// -*-C++-*-

/** $Revision$
*** $Date$
*** $Author$
*** $Source$
**/

#ifndef __GNUC__
// Implementation stuff included here because g++
// (rightly) doesn't do implicit .cxx file inclusion.

#include <sys/types.h>
#include "stdlib.h"
#include "btree.h"


template <class BINARY_NODE>
BINARY_TREE_NODE<BINARY_NODE>* BINARY_TREE_NODE<BINARY_NODE>::
  Enter(BINARY_NODE node, MEM_POOL *pool) 
{
  //typedef BINARY_TREE_NODE<BINARY_NODE> THIS_NODE;
  //typedef BINARY_TREE_NODE<BINARY_NODE> *THIS_NODEP;
  //THIS_NODEP nodep = this;
  BINARY_TREE_NODE<BINARY_NODE> *nodep = this;
  BOOL found = FALSE;
  while (!found) {
    if (nodep->_data == node) {
      found = TRUE;
    } else if (node < nodep->_data) {
      if (!nodep->_left) {
	nodep->_left = CXX_NEW(BINARY_TREE_NODE<BINARY_NODE>(node),pool);
	found = TRUE;
      }
      nodep = nodep->_left;
    } else {
      if (!nodep->_right) {
	nodep->_right = CXX_NEW(BINARY_TREE_NODE<BINARY_NODE>(node),pool);
	found = TRUE;
      }
      nodep = nodep->_right;
    }
  }
  return nodep;
}
   

template <class BINARY_NODE>
BINARY_TREE_NODE<BINARY_NODE>* BINARY_TREE_NODE<BINARY_NODE>::
  Find(BINARY_NODE node) const
{
  //typedef BINARY_TREE_NODE<BINARY_NODE> *THIS_NODEP;
  BINARY_TREE_NODE<BINARY_NODE> *nodep = (BINARY_TREE_NODE<BINARY_NODE> *)this;
  while (1) {
    if (nodep->_data == node) {
      return nodep;
    } else if (node < nodep->_data) {
      if (nodep->_left) {
	nodep = nodep->_left;
      } else {
	return NULL;
      }
    } else {
      if (nodep->_right) {
	nodep = nodep->_right;
      } else {
	return NULL;
      }
    }
  }
}

#endif
