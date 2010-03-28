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
 * Module: driver_util.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  17-Feb-95 - Original Version
 *
 * Description:
 *   supporting functions for the backend driver.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef driver_util_INCLUDED
#define driver_util_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

extern void Get_Phase_Args (BE_PHASES, INT*, char ***);

extern void Process_Command_Line (INT, char **);

extern void Prepare_Source (void);

extern void Prepare_Listing_File (void);

extern void Lowering_Initialize (void);

extern void Lower_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* driver_util_INCLUDED */
