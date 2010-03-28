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


/* =======================================================================
 * =======================================================================
 *
 *  Module: igls.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Description:
 *  ============
 *
 *  Integrated Global Local Scheduler (IGLS) Interface Routines
 *
 * =======================================================================
 * =======================================================================
 */
#ifndef igls_INCLUDED
#define igls_INCLUDED

/* ======================================================================
 * Call the integrated scheduler for the entire PU (region). The 
 * 'before_regalloc' parameter indicates whether we are calling the IGLS
 * interface before register allocation or not.
 * ======================================================================*/
extern void IGLS_Schedule_Region (BOOL before_regalloc);

#endif /* locs_INCLUDED */


