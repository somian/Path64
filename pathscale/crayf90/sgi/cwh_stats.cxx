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
 * Module: cwh_stats.c
 * $Revision: 
 * $Date$
 * $Author$
 * $Source: 
 *
 * Revision history:
 *  dd-mmm-95 - Original Version
 *
 * Description: Accumulates and prints statistics about the
 *              conversion. Essentially a bunch of counters,
 *              and associated strings.
 *
 *              Counters for TYs are defined,
 *              but there's no reason others couldn't be
 *              added. Enums and macros in cwh_stats.h
 *              define the counter groups and cwh_stats.i contains
 *              the local definitions and strings.
 *
 * ====================================================================
 * ====================================================================
 */

static char *source_file = __FILE__;

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


/* sgi includes */

#include "defs.h"
#include "stab.h"
#include "tracing.h"

/* conversion includes */

#include "cwh_stats.h"
#include "cwh_stats.i"

/*================================================================
 *
 * cwh_stats_print
 *
 * Top level routine to print stats. Calls one routine 
 * for each group of items to print. Pass in the PU's ST.
 *
 * Only TY details are printed just now.
 *
 * invoked via -ttIRB:16 
 *
 * ================================================================
 */
extern void 
cwh_stats_print(ST * st) 
{
  CTR * cp ;

  fprintf (TFile,"\n%s (last TY id %d) ",ST_name(st),SYMTAB_last_type_id(Global_Symtab));

  if (ST_pu_is_nested_func(st)) 
    fprintf (TFile," (internal) ");

  fprintf (TFile," \n \n");

  cwh_stats_print_details(&ty_c) ;

}

/*================================================================
 *
 * cwh_stats_print_details
 *
 * Walk through array  of counters 
 *
 * a) print the values
 * b) save current values to compute increments per PU.
 * c) sum the values.
 *
 * ================================================================
 */
static void 
cwh_stats_print_details( CTR * cp ) 
{

  int sum,i,val ;
  sum = 0 ;

  fprintf (TFile,CTR_misc_str(cp,0));

  for (i = 0 ; i < CTR_num(cp) ; i ++) {

    val = CTR_count(cp,i);

    if (CTR_add_to_sum(cp,i))
      sum += val ;

    if (val != 0) 
      fprintf (TFile,CTR_str(cp,i),val,val-CTR_saved(cp,i));

    CTR_saved(cp,i) = val;
  
  }

  fprintf (TFile,CTR_misc_str(cp,1));
  fprintf (TFile,CTR_misc_str(cp,2),sum,sum-CTR_saved_sum(cp)) ;
  CTR_saved_sum(cp) = sum;

}

