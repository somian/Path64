//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_combine.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  15-SEP-94 shin - Original Version
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:  Interface to routines for special handling of
//               combined operations
//
// ====================================================================
// ====================================================================


#ifndef opt_combine_INCLUDED
#define opt_combine_INCLUDED	"opt_combine.h"

// main driver for the code to try to combine operations, which may
// later be split up.  This should just deal with cases in which we
// simplify/change expressions, and later at emit time, we may undo
// the change.  Any real simplifications should be done in the 
// simplifier.
extern BOOL Combine_Operations( WN *old_wn, WN **new_wn );

// main driver for the code to try to un-combine operations which may
// have been created by Combine_Operations.
// Think of this as a targeted simplifier.
extern BOOL Uncombine_Operations( WN *old_wn, WN **new_wn );

#endif  // opt_combine_INCLUDED
