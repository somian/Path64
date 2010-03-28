/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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
// Module: profile_type.h
// $Revision: 1.3 $
// $Date: 04/12/21 14:57:23-08:00 $
// $Author: bos@eng-25.internal.keyresearch.com $ 
// $Source: /home/bos/bk/kpro64-pending/common/com/SCCS/s.profile_type.h $
//
// Revision history:
//  1-Nov-2002 - Original Version (by s.x.yang)
//
// Description:
// ===========
//     Definition of profiling type and phase. 
//
// ====================================================================
// ====================================================================

#ifndef profile_type_INCLUDED
#define profile_type_INCLUDED

#ifndef INT32_MAX
    # define INT32_MAX  2147483647
#endif

// When to instrument?  Correlates to Instrumentation_Phase_Num
enum PROFILE_PHASE
{
  PROFILE_PHASE_NONE            = -1,
  PROFILE_PHASE_BEFORE_VHO	= 0,
  PROFILE_PHASE_IPA_CUTOFF	= 0,	// phases less than or equal to
					// IPA_CUTOFF will not be
					// instrumented when the input file 
					// is an ipa-generated file.
  PROFILE_PHASE_BEFORE_LNO	= 1,
  PROFILE_PHASE_BEFORE_WOPT	= 2,
  PROFILE_PHASE_BEFORE_CG	= 3,
  PROFILE_PHASE_BEFORE_REGION	= 4,
  PROFILE_PHASE_LAST		= 5,
  PROFILE_PHASE_MAX             = INT32_MAX  // Fb_Hdr size must be 0 mod 64
};


// What instrument? Correlates to Profile_Type
enum PROFILE_TYPE
{
  WHIRL_PROFILE     = 1,
  CG_EDGE_PROFILE   = 2,
  CG_VALUE_PROFILE  = 4,
  CG_STRIDE_PROFILE  = 8,
  PROFILE_TYPE_LAST = 16,
  PROFILE_TYPE_MAX  = INT32_MAX
};

#endif /*profile_type_INCLUDED*/
