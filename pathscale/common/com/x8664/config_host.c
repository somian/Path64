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



static char *source_file = __FILE__;
static char *rcs_id = "$Source$ $Revision$";

#include "defs.h"
#include "config.h"

/* ====================================================================
 *
 * Preconfigure_Host
 *
 * Configuration of host-specific parameters, prior to flag processing.
 *
 * ====================================================================
 */

void
Preconfigure_Host ( void )
{
#if HOST_IS_LITTLE_ENDIAN
  Host_Byte_Sex = LITTLE_ENDIAN;
#else
  Host_Byte_Sex = BIG_ENDIAN;
#endif

  return;
}


/* ====================================================================
 *
 * Configure_Host
 *
 * Configuration of host-specific parameters, after flag processing.
 *
 * ====================================================================
 */

void
Configure_Host ( void )
{
  return;
}



/* ====================================================================
 *
 * Configure_Source_Host
 *
 * Reconfiguration of host-specific parameters for each source file.
 *
 * ====================================================================
 */

/*ARGSUSED*/
void
Configure_Source_Host ( char *filename )
{
  return;
}
