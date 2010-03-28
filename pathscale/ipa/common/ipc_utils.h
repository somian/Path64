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


/* ====================================================================
 * ====================================================================
 *
 * Module: ipc_utils.h
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:37-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /hosts/bonnie.engr/depot/cmplrs.src/mongoose/ipa/common/RCS/ipc_utils.h,
v $
 *
 * Revision history:
 *  31-Jul-95 - Original Version
 *
 * Description:
 *
 * Supporting routines for processing libraries in cross_file STANDALONE inlining
 *
 * ====================================================================
 * ====================================================================
 */


#ifndef ipc_utils_INCLUDED
#define ipc_utils_INCLUDED


#include <ar.h>
#include "mempool.h"

extern void *Digest_Archive (void* handle, MEM_POOL* m, INT64 file_size);
extern void Cleanup_Archive_Handle (void *handle, MEM_POOL*);
extern off_t Defined_By_Archive (char *name, void* handle);
extern char *Read_Member_Name (struct ar_hdr *header, void* handle, MEM_POOL* m);
extern off_t Next_Archive_Member (char* base, off_t offset, INT64 size);

#endif // ipc_utils_INCLUDED
