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


/* =======================================================================
 * =======================================================================
 *
 *  Module: vector.c
 *  $Revision: 1.5 $
 *  $Date: 04/12/21 14:57:05-08:00 $
 *  $Author: bos@eng-25.internal.keyresearch.com $
 *  $Source: /home/bos/bk/kpro64-pending/be/cg/SCCS/s.cg_vector.cxx $
 *
 *  Description:
 *  ============
 *
 *  Package for creating and manipulating a vector of 'void *' elements.
 *
 * =======================================================================
 * =======================================================================
 */

#include <stdlib.h>
#include "defs.h"
#include "mempool.h"
#include "errors.h"
#include "cg_vector.h"


VECTOR
VECTOR_Init (INT size, MEM_POOL *pool)
{
  VECTOR v = TYPE_MEM_POOL_ALLOC (VECTOR_STRUCT, pool);
  v->vector = TYPE_MEM_POOL_ALLOC_N (void *, pool, size);
  VECTOR_size(v) = size;
  VECTOR_count(v) = 0;
  return v;
}


void
VECTOR_Add_Element (VECTOR vector, void *element)
{
  INT count = VECTOR_count(vector);
  FmtAssert (count < VECTOR_size(vector), ("VECTOR overflow"));
  VECTOR_element(vector, count) = element;
  count++;
  VECTOR_count(vector) = count;
}


BOOL
VECTOR_Member_Element (VECTOR vector, void *element)
{
  INT i, count = VECTOR_count(vector);
#ifdef KEY
  FmtAssert (count <= VECTOR_size(vector), ("VECTOR overflow"));
#else
  FmtAssert (count < VECTOR_size(vector), ("VECTOR overflow"));
#endif // KEY
  
  for (i = 0; i < count; i++) {
    if (VECTOR_element(vector,i) == element) {
      return TRUE;
    }
  }
  
  return FALSE;
}

void
VECTOR_Sorted_Add_Element (
  VECTOR vector, 
  void *element, 
  VECTOR_ELEMENT_COMPARE compare_func)
{
  INT i;
  INT count = VECTOR_count(vector);
  FmtAssert (count < VECTOR_size(vector), ("VECTOR overflow"));
  for (i = count; i > 0; i--) {
    void *cur_element = VECTOR_element(vector,i-1);
    if (compare_func (cur_element, element) > 0) break;
    VECTOR_element(vector,i) = cur_element;
  }
  VECTOR_element(vector, i) = element;
  count++;
  VECTOR_count(vector) = count;
}


void
VECTOR_Sort (VECTOR vector, VECTOR_ELEMENT_COMPARE compare_func)
{
  qsort (
    &VECTOR_element(vector,0), 
    VECTOR_count(vector), 
    sizeof (void *),
    compare_func);
}

void 
VECTOR_Reset (VECTOR vector)
{
  VECTOR_count(vector) = 0;
}

void
VECTOR_Delete_Element (VECTOR vector, void *element)
{
  INT i, j;
  INT count = VECTOR_count(vector);

  for (i = 0; i < count; i++) {
    if (VECTOR_element(vector,i) == element) {
      for (j = i+1; j < count; j++) {
	VECTOR_element(vector, j-1) = VECTOR_element(vector, j);
      }
      VECTOR_count(vector) = count-1;
      return;
    }
  }
}
