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
 * Module: bbregs.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  21-may-93 - Original Version
 *
 * Description:	header file for extended BB-related data structures
 * in the new implementation of GRA 
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef bbregs_INCLUDED
#define bbregs_INCLUDED

#ifdef _KEEP_RCS_ID
static char *bbregs_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "gtn_set.td"
#include "register.h"


typedef struct bbregs {
  GTN_SET	*defreach_in;	    /* Some def reaches top */
  GTN_SET	*defreach_out;      /* Some def reaches bottom */
  GTN_SET	*live_in;           /* Some use exposed to top */
  GTN_SET	*live_out;          /* Some use exposed to bottom */
  GTN_SET       *live_use;          /* TNs with upward exposed uses in bb */
  GTN_SET       *live_def;          /* TNs defined above all uses in same bb */
} BBREGS;

/* define the access macros from BBREGS */
#define BBREGS_defreach_in(bbr)		((bbr)->defreach_in)
#define BBREGS_defreach_out(bbr)	((bbr)->defreach_out)
#define BBREGS_live_in(bbr)		((bbr)->live_in)
#define BBREGS_live_out(bbr)		((bbr)->live_out)
#define BBREGS_live_use(bbr)		((bbr)->live_use)
#define BBREGS_live_def(bbr)		((bbr)->live_def)


/* access macros from BB */
#define BB_defreach_in(bb)	BBREGS_defreach_in(BB_bbregs(bb))
#define BB_defreach_out(bb)	BBREGS_defreach_out(BB_bbregs(bb))
#define BB_live_in(bb)		BBREGS_live_in(BB_bbregs(bb))
#define BB_live_out(bb)		BBREGS_live_out(BB_bbregs(bb))
#define BB_live_use(bb)		BBREGS_live_use(BB_bbregs(bb))
#define BB_live_def(bb)		BBREGS_live_def(BB_bbregs(bb))


#endif /* bbregs_INCLUDED */
