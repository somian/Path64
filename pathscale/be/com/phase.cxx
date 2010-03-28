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
 * Module: phase.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  17-Feb -95 - Original Version
 *
 * Description:
 *  Phase-specific functions and data structures that the common driver
 *  need to know.
 *
 * ====================================================================
 * ====================================================================
 */

#include "defs.h"
#include "phase.h"

PHASE_SPECIFIC_OPTION_GROUP phase_ogroup_table[] = {
    {"SWP", 3, PHASE_CG},
    {"GRA", 3, PHASE_CG},
    {"CG", 2, PHASE_CG},
    {"GCM", 3, PHASE_CG},
    {"IPL", 3, PHASE_IPL},
    {NULL}			    /* list terminator */
};
