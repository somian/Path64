/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


#pragma ident "@(#) libu/ffio/class_mr_sds_check.c	92.1	06/29/99 13:16:47"
#include <ffio.h>
#include <stdio.h>
#include <liberrno.h>
#include "layer_def.h"
#include "spec_parse.h"

/*
 * This function verifies that the options given with assign -F [mr,sds]
 * are correct. Some verification of options is done before this is called.
 * For example, we know that no bogus options have been selected. But 
 * other options must be looked at all together, and that's what this
 * function does.
 */
_mr_sds_check(
union spec_u *specs,	/* the options */
struct LAYER_DATA *table,	/* the cdc table */
int num,		/* number of valid specs */
int warnmode,
int errmode)
{

union spec_u *spec2;
    if (specs->class_info.class == CLASS_MR ||
        specs->class_info.class == CLASS_SDS) {
        if (num >= 2) {
            specs++;
            spec2 = specs+1;
            if ((specs->info.quan != 0) && (spec2->info.quan != 0)){
                if (specs->info.quan > spec2->info.quan) {
                    if (errmode)
                        _lerror(_LELVL_MSG, ERAS_INMAX, 
                            specs->info.quan,spec2->info.quan);

                    return(ERR);
                }
            }
        }
    }
    return(0);
}
