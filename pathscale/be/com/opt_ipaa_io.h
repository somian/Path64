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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
// ====================================================================
// ====================================================================
//
// Module: opt_ipaa_io.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  22-Nov-95 - Original Version
//
// Description:
//
// See also opt_ipaa_summary.(h|cxx) for general definitions of the
// IPAA/WOPT interface data structures.  This file contains just those
// I/O routines which must reside in be.so because they are called
// before wopt.so is loaded.
//
// ====================================================================
// ====================================================================

#ifndef cxx_opt_ipaa_io_INCLUDED
#define cxx_opt_ipaa_io_INCLUDED

#ifdef _KEEP_RCS_ID
static char *opt_ipaa_io_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

extern "C" {
  // Read table from .B file
  void *IPAA_LOCAL_MAP_Read ( char *base, Elf64_Word size );

  // Write the table to the .B file
  void IPAA_LOCAL_MAP_Write ( void *localmap, struct output_file *fl );
}

// Global pointer to local-map for use in the back-end --
// defined in common/com/ir_bcom.c.
extern "C" void *IPAA_Local_Map;

#endif
