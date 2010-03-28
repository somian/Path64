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


/* USMID @(#) libu/ffio/cos_parse.h	92.0	10/08/98 14:57:41 */

/*  COS LAYER ASSIGN PARSING DEFINITION */

#include "cosio.h"
#define NUM_COS_ALIAS    0

struct LAYER_OPTS _cos_opts[] = {
 CLASS_COS, _STR1M, _INFO1_STR1(TR_AUTO),  0, 0, _STR1M, 0, 0, "auto" ,
 CLASS_COS, _STR1M, _INFO1_STR1(TR_SYNC),  0, 0, _STR1M, 0, 0, "sync" ,
 CLASS_COS, _STR1M, _INFO1_STR1(TR_ASYNC), 0, 0, _STR1M, 0, 0, "async"
} ;

struct LAYER_NUMERICS _cos_numerics[]  = {
   1, 0, 'b' , 1 , 0xfffff  , 0 ,  "bufsize" /*  number of blocks for buffer */
} ;

#define NUM_COS_NUMERICS (sizeof(_cos_numerics)/sizeof(struct LAYER_NUMERICS))
#define NUM_COS_OPTS     (sizeof(_cos_opts)/sizeof(struct LAYER_OPTS))

struct LAYER_DATA _cos_data =
    {
         CLASS_COS ,
         FLD_TYPE,
         "cos",
         "" ,   /* ".auto" */
         0,
         0,
         NUM_COS_OPTS,
         1 ,
         NUM_COS_NUMERICS,
         NUM_COS_ALIAS ,
         _cos_opts ,
         _cos_numerics ,
         NULL,
         NULL
    } ;

struct LAYER_DATA _blocked_data =
    {
         CLASS_COS ,
         FLD_TYPE,
         "blocked",
         "" ,   /* ".auto" */
         0,
         0,
         NUM_COS_OPTS,
         1 ,
         NUM_COS_NUMERICS,
         NUM_COS_ALIAS ,
         _cos_opts ,
         _cos_numerics ,
         NULL,
         NULL
    } ;
