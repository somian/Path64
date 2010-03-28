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
 *  Module: vector.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Package for creating and manipulating a vector of 'void *' elements.
 *
 * =======================================================================
 * =======================================================================
 */

#ifndef vector_INCLUDED
#define vector_INCLUDED

#include "defs.h"
#include "mempool.h"

typedef struct {
  void **vector;
  INT16 count;
  INT16 size;
} VECTOR_STRUCT, *VECTOR;

#define VECTOR_element(v,i)	((v)->vector[i])
#define VECTOR_count(v)		((v)->count)
#define VECTOR_size(v)		((v)->size)

typedef INT (*VECTOR_ELEMENT_COMPARE)(const void*, const void*);

extern VECTOR VECTOR_Init (INT size, MEM_POOL *pool);

extern void VECTOR_Add_Element (VECTOR vector, void *element);

extern BOOL VECTOR_Member_Element (VECTOR vector, void *element);

extern void VECTOR_Sorted_Add_Element (
  VECTOR vector, 
  void *element, 
  VECTOR_ELEMENT_COMPARE compare_func);

extern void VECTOR_Sort (VECTOR vector, VECTOR_ELEMENT_COMPARE compare_func);

extern void VECTOR_Delete_Element (VECTOR vector, void *element);

extern void VECTOR_Reset (VECTOR vector);

#endif /* vector_INCLUDED */

