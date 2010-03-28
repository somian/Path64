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
// =======================================================================
// =======================================================================
//
//  Module: cio_rwtran.h
//  $Revision$
//  $Date$
//  $Author$
//  $Source$
//
//  Revision comments:
//
//  5-Jun-1995 - Initial version
//
//  Description:
//  ============
//  This module consists of cross iteration vector read/cse/write removal.
//  Currently it assumes that if conversion has been done and inner loop
//  body BB reflects this fact.
//
// 
//  Exported Functions:
//  ==================+
//
//    BOOL CIO_Read_Write_Removal(LOOP_DESCR *loop)
//       Main driver for cross iteration read/cse/write removal, where
//       <loop> describes the inner loop. Returns TRUE iff any changes
//       were many to the loop.
//
// =======================================================================
// ======================================================================= */


#ifndef cio_rwtran_INCLUDED
#define cio_rwtran_INCLUDED

#include "findloops.h"


/* external functions */

extern BOOL Perform_Read_Write_Removal( LOOP_DESCR *loop );

#endif /* cio_rwtran_INCLUDED */
