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
 * $Source$
 *
 * Revision history:
 *  25-Mar-97 - Original Version
 *
 * Description:
 *
 * Configure the -PROMP option groups (included in config.c),
 * used by whirl2f.so.
 *
 * ====================================================================
 * ====================================================================
 */

/* This file is included in config.c, so it doesn't need its own set of
 * standard includes -- only the following:
 */
#include "config_promp.h"
 

PROMP_FLAGS Promp_Flags =
{
   FALSE, /* Enabled? */
   FALSE, /* emit owhile? */
   FALSE, /* Show progress */
   NULL,  /* Analysis file name */
   NULL,  /* Original source name */
   1ULL,  /* next_id */
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* buffer[16] */
}; /* Promp_Flags */


PROMP_FLAGS *Current_PROMP = &Promp_Flags;


/* ====================================================================
 * Descriptor for the -LNO option group.
 *
 * See common/util/flags.h, typedef struct option_desc for documentation
 * ====================================================================
 */

#define PRF Promp_Flags
static OPTION_DESC Options_PROMP[] =
{ { OVK_BOOL,	OV_VISIBLE,	FALSE, "",          NULL,
       0, 0, 0, &PRF.enabled,        NULL,
       "Emission of file describing compiler transformations" },
  { OVK_BOOL,	OV_VISIBLE,	FALSE, "owhile",          NULL,
       0, 0, 0, &PRF.owhile,        NULL,
       "Emission of file describing compiler transformations" },
  { OVK_UINT64,	OV_VISIBLE,	FALSE, "next_id",          NULL,
       1, 0, INT32_MAX, &PRF.next_id,        NULL,
       "Next construct identifier number to be defined" },
  { OVK_BOOL,	OV_INTERNAL,	FALSE, "show",      NULL,
       0, 0, 0, &PRF.show,           NULL,
       "Show progress of emission of analysis file" },
  { OVK_NAME,	OV_SHY,		FALSE, "anl",               NULL,
       0, 0, 0, &PRF.anl_filename,   NULL,
       "Gives an alternate name for analysis file" },
  { OVK_NAME,	OV_SHY,		FALSE, "src",               NULL,
       0, 0, 0, &PRF.src_filename,   NULL,
       "Gives an alternate name for original file" }, 
  { OVK_COUNT }			    /* List terminator -- must be last */
}; /* Options_PROMP */
#undef PRF

