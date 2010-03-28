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


/* USMID @(#) libu/ffio/nosve_parse.h	92.0	10/08/98 14:57:41 */

/*  NOSVE LAYER ASSIGN PARSING DEFINITION */

#include "gxio.h"
#include "gfio.h"
#include "gvio.h"
#include "common_parse.h"
#include "spec_parse.h"		/* for declaration of _class_vf_check */

#define NUM_NOSVE_ALIAS    0

struct LAYER_OPTS _nosve_opts[] = {
 CLASS_X, _STR1M,  _INFO1_STR1(TR_NVE_V),    0, 0, _STR1M, 0, 0,  "v" ,
 CLASS_V, _STR1M,  _INFO1_STR1(TR_NVE_S),    0, 0, _STR1M, 0, 0,  "s" ,
 CLASS_V, _STR1M,  _INFO1_STR1(TR_NVE_D),    0, 0, _STR1M, 0, 0,  "d" ,
 CLASS_F, _STR1M,  _INFO1_STR1(TR_VMS_F_TP), 0, 0, _STR1M, 0, 0,  "f" ,
 CLASS_V, _STR1M,  _INFO1_STR1(TR_IBM_U),    0, 0, _STR1M, 0, 0,  "u" 
} ;

#define NUM_NOSVE_OPTS     (sizeof(_nosve_opts)/sizeof(struct LAYER_OPTS))


struct LAYER_DATA _nosve_data =
    {
         CLASS_X ,
         FLD_TYPE     ,
         "nosve" ,
         "" ,
         _STR1M,
         0,
         NUM_NOSVE_OPTS,
         1 ,
         NUM_REC_BUF_NUMERICS,
         NUM_NOSVE_ALIAS ,
         _nosve_opts ,
         _rec_buf_numerics ,
         NULL,
         _class_vf_check
    } ;

