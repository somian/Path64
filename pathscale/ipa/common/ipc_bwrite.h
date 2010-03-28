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


#ifndef ip_bwrite_INCLUDED
#define ip_bwrite_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// The procedure for binary output: call IP_WRITE_pu for every pu in
// the program, and then, at some point before IPA terminates, call
// IP_flush_output and IP_write_global_symtab.  

// Forward declaration of a struct defined in ipc_file.h
struct IP_FILE_HDR;

extern void IP_WRITE_pu(IP_FILE_HDR *s, INT pindex);
extern void IP_flush_output(void);
extern char* IP_global_symtab_name(void);
extern void IP_write_global_symtab(void);

#ifdef __cplusplus
}
#endif

#endif /* ip_bwrite_INCLUDED */
