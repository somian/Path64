/*
 *  Copyright (C) 2007. Pathscale, LLC. All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
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


//-*-c++-*-

/**
*** Module: cxx_graph.cxx
*** $Revision$
*** $Date$
*** $Author$
*** $Source$
*** 
*** Revision history:
*** 
***     21-OCT-94 dkchen - Original Version
***     10-NOV-94 dkchen - Make DIRECTED_GRAPH16 a template class and move
***                        it to graph_template.h.
*** 
*** Description:
*** 
*** This file contains definitions for fuctions in the VERTEX16 and EDGE16
*** classes.
*** 
**/

#include "cxx_graph.h"
#include "cxx_memory.h"
#include "errors.h"

const VINDEX16 INVALID_VINDEX16=0xffff; //increase from 0xffff for bug 13018
VINDEX16 GRAPH16_CAPACITY = 0xfffe; //increase from 0xfffe for bug 13018

VERTEX16&
VERTEX16::operator=(const VERTEX16& v) {

  // copy evverything

  _from = v._from;
  _to = v._to;

  return *this;
}

EDGE16&
EDGE16::operator=(const EDGE16& e) {

  // copy evverything

  _from = e._from;
  _to = e._to;
  _nfrom = e._nfrom;
  _nto = e._nto;

  return *this;
}

