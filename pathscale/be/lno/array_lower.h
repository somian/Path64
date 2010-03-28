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


//-*-C++-*-
/* ====================================================================
 * ====================================================================
 *
 * Module: array_lower.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description:
 * 
 * 
 *                      Array Lowering
 *                      --------------
 * 
 *    This file lowers all accesses to reshaped array variables.  
 *    
 *    Each access to a distributed dimension is translated as follows:
 *    
 *        Cyclic(b) distributions, the general case: block size = b, procs = p
 * 
 *            A[j] ==> A[(j/b)%p][j/pb][j%b] 
 *        
 *            j/b%p = processor on which element "j" is allocated
 *            j/pb  = block within the processor 
 *            j%b   = offset within the block
 * 
 *    We can optimize the array index expressions for the following 
 *    special cases:
 * 
 *        Block distributions: block size b = divceil(N,p), where N is the 
 *        size of the dimension
 * 
 *            A[j] ==> A[j/b][j%b]
 * 
 *            j/b = processor on which element "j" is allocated
 *            j%b = offset within the block
 *    
 *        Cyclic(1) distributions: block size = 1
 * 
 *            A[j] ==> A[j%p][j/p]
 * 
 *            j%p = processor on which element "j" is allocated
 *            j/p = offset within the block
 *    
 * 
 *  Exported types and functions:
 *
 *      void Lower_Array_Accesses(WN *func_nd);
 *
 *          Lower all the accesses to reshaped arrays in func_nd, where
 *          func_nd is an OPR_FUNC_ENTRY node.
 *
 * ====================================================================
 * ====================================================================
 */
#ifndef array_lower_INCLUDED
#define array_lower_INCLUDED

#ifdef _KEEP_RCS_ID
static char *array_lower_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "wn.h"

extern void Lower_Array_Accesses(WN *func_nd);

#endif /* array_lower_INCLUDED */

