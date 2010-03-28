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
 * Module: be_config_targ.cxx
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  10-Feb-95 - Original Version
 *
 * Description:
 *   Backend-specific routines moved from common/com/MIPS/config_targ.c.
 *   Refer to config_targ.c for details.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "config.h"
#include "config_targ.h"
#include "mtypes.h"
#include "stab.h"
#include "targ_sim.h"
#include "ti_init.h"

/* ====================================================================
 *
 * BE_Configure_Target
 *
 * Configuration of target-specific parameters used only by the backend,
 * after flag processing. 
 *
 * ====================================================================
 */
void
BE_Configure_Target (void)
{
}


/* ====================================================================
 *
 * BE_Configure_Source_Target
 *
 * Reconfiguration of target-specific, backend-specific parameters for each
 * source file.
 *
 * ====================================================================
 */
void
BE_Configure_Source_Target (void)
{
}
