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
// ====================================================================
// ====================================================================
//
//  Module: cg_grouping.h
//  $Revision: 1.4 $
//  $Date: 05/03/02 15:26:47-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: be/cg/mips/SCCS/s.cg_grouping.h $
//
//  Synopsis:
//
//      A package for inquiring about grouping of ops, where the aim is
//      to group a given set of ops such that we do not get an issue-split.
//      This interface combines information from the targ_isa_bundle.h
//      interface and the ti_si.h interface. I.e. it combines bundling
//      and resource information into grouping information.
//
//  Interface Description:
//
//	Exported types:
//
//      Exported functions:
//
//      Usage:
//
//         This interface should only be used within the IA-64 target
//         directory, and all defs are therefore in the header file
//         such that we do not have to introduce cg_grouping to other
//         target directories.
//
// ====================================================================
// ====================================================================

#ifndef cg_grouping_INCLUDED
#define cg_grouping_INCLUDED

#include <vector>
#include "mempool_allocator.h"
#include "ti_si.h"
#include "topcode.h"

typedef mempool_allocator<INT32>           INT32_MEMALLOC;
typedef std::vector<INT32, INT32_MEMALLOC> INT32_VECTOR;

class CG_GROUPING
{
private:

public:
}; // class CG_GROUPING
  

#endif // cg_grouping_INCLUDED
