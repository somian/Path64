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
 * Module: be_version.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  06-Dec -95 - Original Version
 *
 * Description:
 *    Utilities for all backend components.
 *
 * ====================================================================
 * ====================================================================
 */

/* Updated the BE_Version if the interface between be.so and wopt.so
 * (or any other component) is changed.
 * 
 *  e.g., POINTS_TO analysis is now defined in be.so and used by wopt.so
 *  If POINTS_TO data structure are changed, the BE_VERSION in config.h 
 *  should be updated.
 */
#define BE_VERSION "1.01"           /* define current be version */

#ifdef __cplusplus
extern "C" const char *Get_BE_Version(void);	/* version of be and be.so */
#else
extern const char *Get_BE_Version(void);	/* version of be and be.so */
#endif
