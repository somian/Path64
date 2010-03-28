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


/* USMID @(#) libu/ffio/ibm_parse.h	92.0	10/08/98 14:57:41 */

/*  IBM LAYER ASSIGN PARSING DEFINITION */

#include "common_parse.h"
#include "gfio.h"
#include "gvio.h"
#define NUM_IBM_ALIAS    0

struct LAYER_OPTS _ibm_opts[] = {
 CLASS_V, _STR1M, _INFO1_STR1(TR_IBM_U),   0, 0, _STR1M, 0, 0,  "u" ,
 CLASS_V, _STR1M, _INFO1_STR1(TR_IBM_V),   0, 0, _STR1M, 0, 0,  "v" ,
 CLASS_V, _STR1M, _INFO1_STR1(TR_IBM_VB),  0, 0, _STR1M, 0, 0,  "vb" ,
 CLASS_V, _STR1M, _INFO1_STR1(TR_IBM_VBS), 0, 0, _STR1M, 0, 0,  "vbs" ,
 CLASS_F, _STR1M, _INFO1_STR1(TR_IBM_F),   0, 0, _STR1M, 0, 1,  "f" ,  /* special case */
 CLASS_F, _STR1M, _INFO1_STR1(TR_IBM_FB),  0, 0, _STR1M, 0, 1,  "fb" 
} ;

#define NUM_IBM_OPTS     (sizeof(_ibm_opts)/sizeof(struct LAYER_OPTS))
extern int _class_vf_check(union spec_u *specs, struct LAYER_DATA *table,
    int num, int warnmode, int errmode);


struct LAYER_DATA _ibm_data =
    {
         CLASS_V ,
         FLD_TYPE,
         "ibm",
         "" ,
         0x00070000,
	 0,
         NUM_IBM_OPTS,
         1 ,
         NUM_REC_BUF_NUMERICS,
         NUM_IBM_ALIAS ,
         _ibm_opts ,
         _rec_buf_numerics ,
         NULL,
         _class_vf_check
    } ;

