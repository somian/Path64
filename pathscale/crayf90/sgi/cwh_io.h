/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 * Module: cwh_io.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef CWH_IO_INCLUDED
#define CWH_IO_INCLUDED
#ifdef _KEEP_RCS_ID
  static char *rcs_id = " $Id$ ";
#endif /* _KEEP_RCS_ID */

extern INT cwh_io_in_ioblock; /* are we processing an I/O statement? */

#ifdef KEY /* Bug 7443 */
/* Are we inside an I/O statement? */
extern int inside_io;
#endif /* Bug 7443 */

#endif /* CWH_IO_INCLUDED */
