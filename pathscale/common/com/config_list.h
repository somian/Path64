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
 * Module: config_list.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  18-Mar-97 - Original Version
 *
 * Description:
 *
 * Global variables holding -LIST option group settings.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef config_list_INCLUDED
#define config_list_INCLUDED

#ifdef _KEEP_RCS_ID
/*REFERENCED*/
static char *config_list_h_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


/* ====================================================================
 * List of global variables that are set by the -LIST option group
 * ====================================================================
 */

/***** Listing options *****/
extern BOOL List_Enabled;	/* -LIST:=ON */
extern BOOL List_Cite;		/* -LIST:cite - Cite tool support */
extern BOOL List_Options;	/* -LIST:options - option listing */
extern BOOL List_All_Options;	/* -LIST:all_options - option listing */
extern BOOL List_Source;	/* -LIST:source - source code listing */
extern BOOL List_Symbols;	/* -LIST:symbols - symbol table listing */
extern BOOL List_Notes;		/* -LIST:notes - in .s file */
extern BOOL List_Software_Names; /* -LIST:software_names - in .s file */

#endif /* config_list_INCLUDED */
