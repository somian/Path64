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


#ifndef __IPC_DEFS_H__
#define __IPC_DEFS_H__

#ifdef _64BIT_OBJECTS
#define NAME(n) n##64
#define QUOTENAME(n) n "64"
#else
#define NAME(n) n##32
#define QUOTENAME(n) n "32"
#endif

/* used in ipc_link.c */
#define DEFAULT_MAX_COMMA_LIST 32

/* used in ipc_compile.c */
#define IPACOM_NAME "/ipacom"

typedef enum ip_target_type {
    IP_32_bit_ABI,
    IP_64_bit_ABI
} IP_TARGET_TYPE;

extern IP_TARGET_TYPE IPA_Target_Type;

#endif /* __IPC_DEFS_H__ */
