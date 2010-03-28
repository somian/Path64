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


// This may look like C code, but it is really -*- C++ -*-

#ifndef _ara_utils_INCLUDED
#define _ara_utils_INCLUDED

#include "lnopt_main.h"

extern void print_indent(FILE *fp, INT indent);

extern void Merge_Scalar_List(SCALAR_STACK* st_from,
                              SCALAR_STACK* st_to);

extern void Merge_Scalar_List_Covered(SCALAR_STACK* st_from,
                                      ARA_LOOP_INFO* ali_to,
                                      SCALAR_STACK* st_to_covered,
                                      SCALAR_STACK* st_to_not_covered);

extern POINTS_TO* Points_To(ARA_REF* ara_ref, 
                            MEM_POOL* mem_pool);

extern POINTS_TO* Points_To(SCALAR_NODE* sn,
			    MEM_POOL* mem_pool);

extern POINTS_TO* Points_To(WN* wn, 
			    MEM_POOL* mem_pool);

#endif
