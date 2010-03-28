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


#pragma ident "@(#) libu/ffio/class_vf_check.c	92.1	06/29/99 13:16:47"
#include <ffio.h>
#include <stdio.h>
#include <liberrno.h>
#include "layer_def.h"
#include "spec_parse.h"
#include "gfio.h"
#include "gvio.h"
extern struct v_rec_limit_s _V_limits[];
extern struct f_rec_limit_s _F_limits[];

/*
 * This function verifies that the options given with assign -F [ibm,nosve]
 * are correct. Some verification of options is done before this is called.
 * For example, we know that no bogus options have been selected. But 
 * other options must be looked at all together, and that's what this
 * function does.
 */
_class_vf_check(
union spec_u *specs,	/* the options */
struct LAYER_DATA *table,	/* the cdc table */
int num,		/* number of valid specs */
int warnmode,
int errmode)
{
union spec_u *spec2;
int bdwlen;
int type;
    if (specs->class_info.class == CLASS_V) {
        if (num >= 2) {
            type = specs->hdr.str1;
            specs++;
            spec2 = specs+1;
            bdwlen = 4;
            if (specs->info.quan != 0) {
                if ((specs->info.quan < _V_limits[type].min_rsz) ||
                    (specs->info.quan > _V_limits[type].max_rsz)) {
                    goto badrsiz;
                }
            }
            if (spec2->info.quan != 0) {
                if ((spec2->info.quan < _V_limits[type].min_mbs) ||
                    (spec2->info.quan > _V_limits[type].max_mbs)) {
                    goto badmbs;
                }
            }
            if (specs->info.quan != 0 && spec2->info.quan != 0) {
                switch (type) {
                    case TR_IBM_U:
                        bdwlen = 0;	/* fall through */
                    case TR_IBM_V:
                    case TR_IBM_VB:
                        if (specs->info.quan + bdwlen > spec2->info.quan) {
                           goto badrsiz;
                        }
                        break;
                    case TR_VMS_V_DSK:
                    case TR_VMS_V_TR:
                        if ((specs->info.quan + 2) > spec2->info.quan) {
                            goto badmbs;
                        }
                        break;
                    case TR_VMS_V_TP:
                    case TR_NVE_D:
                        if ((specs->info.quan + 4) > spec2->info.quan) {
                           goto badrsiz;
                        }
                        break;
                }
            }
        }
    }
    else if (specs->class_info.class == CLASS_F) {
        if (num >= 2) {
            int type;
            type = specs->hdr.str1;
            specs++;
            spec2 = specs+1;
            if (GET_VALID_BIT(specs->info) != 0) {
                if ((specs->info.quan == 0) ||
                    (specs->info.quan < _F_limits[type].min_rsz) ||
                    (specs->info.quan > _F_limits[type].max_rsz)) {
                        goto badrsiz;
                }
                if (spec2->info.quan != 0) {
                    if ((spec2->info.quan < _F_limits[type].min_mbs) ||
                        (spec2->info.quan > _F_limits[type].max_mbs)) {
                        goto badmbs;
                    }
                }
                switch (type) {
                    case TR_IBM_F:
                        if ((specs->info.quan != spec2->info.quan &&
                            specs->info.quan != 0)){
                            goto badrsiz;
                        }
                        break;
                    case TR_IBM_FB:
                        if ((spec2->info.quan % specs->info.quan) != 0) {
                            goto badrsiz;
                        }
                        break;
                    case TR_VMS_F_DSK:
                    case TR_VMS_F_TP:
                    case TR_VMS_F_TR:
                        if ((spec2->info.quan != 0) && (specs->info.quan > spec2->info.quan)) {
                            goto badmbs;
                        }
                        break;
                    default:
                        break;
                }
            }
            else {
                goto badrsiz;
            }
        }
    }
    return(0);
badrsiz:
    if (errmode)
       _lerror(_LELVL_MSG, ERAS_BADREC );
    return(ERR);
badmbs:
    if (errmode)
       _lerror(_LELVL_MSG, ERAS_BADMBS );
    return(ERR);
}
