/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


/* USMID @(#) libf/pxf/table.h	92.0	10/08/98 14:30:10 */

#include <fortran.h>
#include <cray/mtlock.h>
#include "pxfstruct.h"

typedef struct table {
  _f_int next_index;
  _f_int size;
  DECL_LOCK(mutex)
  void **entries;
} table_t;

typedef struct pxfhandle_table {
  _f_int next_index;
  _f_int size;
  DECL_LOCK(mutex)
  struct pxfhandle *entries;
} pxfhandle_table_t;

/* global variables for tables */
extern pxfhandle_table_t _pxfhandle_table;
extern table_t _pxfdir_table;

/* functions for maintaining pointer tables */
extern _f_int _table_add(table_t *, void *);
extern int _table_remove(table_t *, _f_int);
extern void *_table_lookup(table_t *, _f_int);

/* functions for maintaining handle tables */
extern _f_int _pxfhandle_table_add(pxfhandle_table_t *, void *, int);
extern int _pxfhandle_table_remove(pxfhandle_table_t *, _f_int);
extern int _pxfhandle_table_replace(pxfhandle_table_t *, _f_int, void *, int);
extern struct pxfhandle _pxfhandle_table_lookup(pxfhandle_table_t *, _f_int);
