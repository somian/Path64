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


#ifndef ipc_partition_INCLUDED
#define ipc_partition_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

   /* This has not yet been ported to NEW_SYMTAB.  
    * Until it is ported I only define
    * the routines needed for ipc_symtab_merge purposes here.  Once
    * ported we probably need to revisit what should or should not be
    * done here, since I assume we will use the merged WHIRL symbol table
    * to do many things that up till now have been done with the ELF
    * symbol table.
    */

#ifdef __cplusplus
}
#endif

#endif /* ipc_partition_INCLUDED */

