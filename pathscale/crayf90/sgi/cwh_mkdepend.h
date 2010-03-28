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
 * Module: cwh_mkdepend.h
 * $Revision$
 * $Date$
 * $Author$ 
 * $Source$
 *
 * Revision history:
 *  09-26-95 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_MKDEPEND_INCLUDED
#define CWH_MKDEPEND_INCLUDED


   
extern char * mdupdate_file;
extern char * mdtarget_file;


/* Generic routines for manipulating tables */
typedef struct {
   INT32 current_idx;  /* Last used index in table */
   INT32 current_size; /* How big is the current table */
   void **ptr;         /* pointer to the data */
} table_s, *table_p;

extern INT32 cwh_next_table_entry( table_p t);
extern void  cwh_add_to_module_files_table(char *name);
extern void  cwh_mkdepend_add_name(INT32 idx, char * name) ;
extern void  cwh_write_makedepend(void) ;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void  cwh_add_to_used_files_table(char * name, INT duplicate) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */




#define TABLE_SIZE(x) (x.current_size)
#define TABLE_CURRENT_IDX(x) (x.current_idx)
#define TABLE_PTR(x) (x.ptr)
#define TABLE_IDX(x,i) ((x).ptr[i])
#define SET_TABLE_IDX(x,i,val) (x).ptr[(i)] = (void *) (val)
#define TABLE_TOP(x,ty) ((ty *) ((x).ptr[(x).current_idx]))
#define TABLE_INCREMENT 16
#define INIT_TABLE {-1,0,NULL}

#endif /* CWH_MKDEPEND_INCLUDED */

