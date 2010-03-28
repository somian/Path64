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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef ipc_bread_INCLUDED
#define ipc_bread_INCLUDED

/*
 * The following routines are responsible for reading the binary WHIRL
 * files into memory.  Typically IP_READ_file_info is used to read in
 * all the global tables, and then IP_READ_pu is used to read each PU.
 * IP_READ_str_table is also available if only the global string table
 * is needed.  These routines can be called in any order -- they
 * automatically read in any prerequisites.  Note that they can also
 * be called multiple times; if the information has already been read,
 * they will just set the global pointers (e.g. Global_Symtab) to the
 * appropriate data structures.
 */


// Read the tree of pu_infos, and then set up the array of
// IP_PROC_INFOs.  The array contains essentially the same information
// as the tree; the main difference is the different data structure.
extern void IP_READ_pu_infos (IP_FILE_HDR& s);

extern void IP_READ_file_info (IP_FILE_HDR& s);

class IPA_NODE;

extern void IP_READ_pu (IPA_NODE* node, IP_FILE_HDR& s, INT p_index,
			MEM_POOL *pool);

#endif /* // ipc_bread_INCLUDED */
