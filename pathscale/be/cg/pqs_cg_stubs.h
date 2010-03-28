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


/* stub definitions for the PQS routines */

#include "defs.h"
#include "pqs_cg.h"

void PQSCG_init(BB * first_bb){}
void PQSCG_reinit(BB * first_bb){}
void PQSCG_term(void){}
BOOL PQSCG_pqs_valid(void){return FALSE;}     
 
BOOL PQSCG_is_disjoint(PQS_TN tn1, PQS_TN tn2){return FALSE;}
BOOL PQSCG_is_disjoint(PQS_TN_SET &tns1, PQS_TN_SET &tns2){return FALSE;}


BOOL PQSCG_is_subset_of (PQS_TN tn1, PQS_TN tn2){return FALSE;}
BOOL PQSCG_is_subset_of (PQS_TN tn1, PQS_TN_SET &tns2){return FALSE;}
BOOL PQSCG_is_subset_of (PQS_TN_SET &tns1, PQS_TN_SET &tns2){return FALSE;}

void PQSCG_copy_tn_map(PQS_TN tn_out, PQS_TN tn_in){}
void PQSCG_add_instruction(PQS_OP op){}
BOOL PQSCG_sets_results_if_qual_true(PQS_OP op) {return TRUE;}

MEM_POOL PQS_mem_pool;
BOOL PQS_disabled;

