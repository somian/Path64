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



#ifndef iface_scn_INCLUDED
#define iface_scn_INCLUDED

#ifdef _KEEP_RCS_ID
static char *iface_scn_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "symtab.h"

/* Incomplete types to keep ANSI happy */
class WN;

/* Initialize anything for the upcoming source file */
extern void Interface_Scn_Begin_File( void );

/* Finalize anything for the just-handled source file */
extern void Interface_Scn_End_File( void );

/* Create an interface descriptor for the entrypoint indicated by
 * the basic block, which must be BB_entry()
 */
extern void Interface_Scn_Add_Def( ST *, WN * );

/* Create an interface descriptor for the given subroutine call */
extern void Interface_Scn_Add_Call( ST *, WN * );


#endif /* iface_scn_INCLUDED */
