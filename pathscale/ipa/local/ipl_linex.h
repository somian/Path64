/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


//* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#ifndef ipl_linex_INCLUDED
#define ipl_linex_INCLUDED

#ifndef loop_info_INCLUDED
#include "loop_info.h"
#endif

#ifndef ipa_section_INCLUDED
#include "ipa_section.h"
#endif

extern ARRAY_SUMMARY Array_Summary;

class SUMMARY_PROCEDURE;

extern void 
IPL_Access_Vector_To_Projected_Region(WN* w, 
                                      SUMMARY_PROCEDURE* proc,
				      INT pu_first_formal_idx,
				      INT pu_last_formal_idx,
                                      INT pu_first_actual_idx, 
                                      INT pu_last_actual_idx,
                                      INT pu_first_callsite_idx, 
                                      INT pu_last_callsite_idx);

#endif
