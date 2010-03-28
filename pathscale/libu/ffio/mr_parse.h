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


/* USMID @(#) libu/ffio/mr_parse.h	92.0	10/08/98 14:57:41 */

/*  MR LAYER ASSIGN PARSING DEFINITION */

#include "fssio.h"
#include "common_parse.h"

#define NUM_MR_ALIAS    0

struct LAYER_OPTS _mr_opts[] = {
   CLASS_MR, _STR1M, _INFO1_STR1(TR_FSS_SCR) , 0, 0, _STR1M, 0, 0, "scr"   ,
   CLASS_MR, _STR1M, _INFO1_STR1(TR_FSS_SAVE), 0, 0, _STR1M, 0, 0, "save"   ,
   CLASS_MR, _STR2M, _INFO1_STR2(FSS_OPT_OVFL),0, 0, _STR2M, 0, 0, "ovfl"   ,
   CLASS_MR, _STR2M, _INFO1_STR2(FSS_OPT_NOVFL),0,0, _STR2M, 0, 0, "novfl"
} ;

#define NUM_MR_OPTS (sizeof(_mr_opts)/sizeof(struct LAYER_OPTS))

extern int _mr_sds_check(union spec_u *specs, struct LAYER_DATA *table, int num,
    int warnmode, int errmode);

struct LAYER_DATA _mr_data = 
    {
         CLASS_MR ,
         FSS_TYPE,
         "mr",
         ".save.ovfl" ,
         0,
         0,
         NUM_MR_OPTS,
         1 ,
         NUM_MR_SDS_NUMERICS ,
         NUM_MR_ALIAS ,
         _mr_opts ,
         _mr_sds_numerics ,
         NULL,
         _mr_sds_check
    } ;

