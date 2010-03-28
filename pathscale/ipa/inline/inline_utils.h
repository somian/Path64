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


#ifndef ip_inline_utils_INCLUDED
#define ip_inline_utils_INCLUDED

#include "dwarf_DST_mem.h"      // Needed by ipc_file.h
#include "ipc_file.h"

extern IP_FILE_HDR&
Setup_Inliner_File_Header (char *input_name, void *mmap_addr);

extern void
Fix_Aliased_Commons ();

#ifdef _LIGHTWEIGHT_INLINER
#include "wn_tree_util.h"

extern BOOL
Need_To_Inline_Callee(WN* w);

extern void
Copy_Input_Info_To_Output(char *, char *);
#endif // _LIGHTWEIGHT_INLINER

#endif // ip_inline_utils_INCLUDED
