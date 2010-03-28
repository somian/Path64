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
 * Module: cgdriver.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  20-Feb-95 - Original Version
 *
 * Description:
 *  
 *  Declaration for exported cg interfaces.
 *
 *  void CG_Configure_Opt_Level( INT opt_level )
 *
 *      Call this to set/change the CG optimization level.  This is the only
 *      valid way to do this.  Directly setting CG_opt_level probably won't do
 *      what you want.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef cgdriver_INCLUDED
#define cgdriver_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Process command line once for cg-specific options */
extern void CG_Process_Command_Line (INT, char **, INT, char **);

extern void CG_Configure_Opt_Level( INT opt_level );

extern void CG_Init (void);	/* init once per compilation */

extern void CG_Fini (void);	/* finalize once per compilation */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* cgdriver_INCLUDED */
