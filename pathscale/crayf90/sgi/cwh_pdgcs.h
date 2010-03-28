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
 * Module: cwh_pdgcs.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Provides prototypes for entry points in cwh_pdgcs.c
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_PDGCS_INCLUDED
#define CWH_PDGCS_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


void PDGCS_initialize ( LANG           language,
                        INT32          init_flags,
                        char          *cmp_name,
                        char          *cmp_rel,
                        char          *obj_file_name,
                        char          *list_file_name,
                        INT32          trunc_bits,
                        INT32          debug_opts,
                        char          *src_path_name,
                        char          *cif_file_name,
                        char          *debug_file_name,
                        FILE          *debug_file,
                        FILE          *cif_file,
                        char          *src_fname,
                        char          *cwd,
                        INT32          n_pes);


void PDGCS_terminate ( int *errors );



#endif /* CWH_PDGCS_INCLUDED */






