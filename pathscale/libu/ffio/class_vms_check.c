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


#pragma ident "@(#) libu/ffio/class_vms_check.c	92.1	06/29/99 13:16:47"
#include <ffio.h>
#include <stdio.h>
#include <liberrno.h>
#include "layer_def.h"
#include "spec_parse.h"
#include "gfio.h"
#include "gvio.h"
#include "vms_def.h"

/*
 * This function sets the str1 and str2 fields of the spec_u
 * structure  and then calls _class_vf_check to verify that the
 * options given with assign -F vms are correct.
 * Some verification of options is done before this is called.
 */
_class_vms_check(
union spec_u *specs,	/* the options */
struct LAYER_DATA *table,	/* the cdc table */
int num,		/* number of valid specs */
int warnmode,
int errmode)
{
int subtype;
int type;
    type = specs->class_info.info1;
    subtype = specs->class_info.info2;
    specs->class_info.info1 = 0;
    if (type == VMS_F) {
        specs->class_info.class = CLASS_F;
        switch(subtype){
            case VMS_TAPE:
                specs->hdr.str1 = TR_VMS_F_TP;
                break;
            case VMS_DSK:
            case VMS_BB:
                specs->hdr.str1 = TR_VMS_F_DSK;
                break;
            case VMS_TR:
                specs->hdr.str1 = TR_VMS_F_TR;
                break;
            default:
                goto badrf;
        }
    }
    else if (type == VMS_S) {
        specs->class_info.class = CLASS_V;
        switch(subtype){
            case VMS_TAPE:
                specs->hdr.str1 = TR_VMS_S_TP;
                break;
            case VMS_DSK:
            case VMS_BB:
            case VMS_NONL:
                specs->hdr.str1 = TR_VMS_S_DSK;
                break;
            case VMS_TR:
                specs->hdr.str1 = TR_VMS_S_TR;
                break;
            default:
                goto badrf;
        }
    }
    else if (type == VMS_V){
        specs->class_info.class = CLASS_V;
        switch(subtype){
            case VMS_TAPE:
                specs->hdr.str1 = TR_VMS_V_TP;
                break;
            case VMS_DSK:
            case VMS_BB:
                specs->hdr.str1 = TR_VMS_V_DSK;
                break;
            case VMS_TR:
                specs->hdr.str1 = TR_VMS_V_TR;
                break;
            default:
                goto badrf;
        }
    }
    else {
         return(ERR);
    }
    specs->class_info.info2 = 0;
    return(_class_vf_check(specs,table,num,warnmode,errmode));
badrf:
    if (errmode)
        _lerror(_LELVL_MSG, ERAS_BADRFMT);
    return(ERR);
}
