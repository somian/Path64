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


static const char source_file[] = __FILE__;
static const char rcs_id[] = "$Source$ $Revision$";

#include <alloca.h>

#include "defs.h"
#include "targ_proc.h"
#include "targ_proc_properties.h"
#include "ti_si.h"

#include "ti_res.h"


/* ====================================================================
 *
 *  TI_RES_Can_Dual_Issue
 *
 *  See interface description
 *
 * ====================================================================
 */
BOOL TI_RES_Can_Dual_Issue(
  TOP    opcode1,
  TOP    opcode2
)
{
  INT min_length, i;
  SI_RR rr1, rr2;

  /* Quick check to see if we could ever dual issue something.
   */
  if (!PROC_has_same_cycle_branch_shadow()) return FALSE;

  /* Resource requirements for each.
   */
  rr1 = TSI_Resource_Requirement(opcode1);
  rr2 = TSI_Resource_Requirement(opcode2);

  /* We only have to check up to the end of the shorter of the two resource
   * requests.
   */
  if ( SI_RR_Length(rr1) < SI_RR_Length(rr2) )
    min_length = SI_RR_Length(rr1);
  else
    min_length = SI_RR_Length(rr2);

  /* Check each cycle.
   */
  for ( i = 0; i < min_length; ++i ) {
    SI_RRW rrw = SI_RRW_Initial();

    rrw = SI_RRW_Reserve(rrw,SI_RR_Cycle_RRW(rr1,i));
    rrw = SI_RRW_Reserve(rrw,SI_RR_Cycle_RRW(rr2,i));
    
    if ( SI_RRW_Has_Overuse(rrw) ) return FALSE;
  }

  return TRUE;
}
