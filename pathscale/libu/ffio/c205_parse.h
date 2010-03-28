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


/* USMID @(#) libu/ffio/c205_parse.h	92.0	10/08/98 14:57:41 */

/*  C205 LAYER ASSIGN PARSING DEFINITION */

#include "gxio.h"
#define NUM_C205_ALIAS    0

struct LAYER_OPTS _c205_opts[] = {
	CLASS_X, _STR1M,  _INFO1_STR1(TR_205_W) , 0, 0, 0, 0, 0, "w"
} ;

struct LAYER_NUMERICS _c205_numerics[]  = {
   1, 0, 'n' , 0 , 0xfffff  , 0 ,  "recsize" ,
   1, 0, 'n' , 0 , 0xfffff  , 0 ,  "bufsize"
} ;

#define NUM_C205_OPTS     sizeof(_c205_opts)/sizeof(struct LAYER_OPTS)
#define NUM_C205_NUMERICS sizeof (_c205_numerics)/sizeof(struct LAYER_NUMERICS)
struct LAYER_DATA _c205_data =
    {
         CLASS_X ,
         FLD_TYPE,
         "c205",
         ".w" ,
         0,
         0,
         NUM_C205_OPTS,
         1 ,
         NUM_C205_NUMERICS,
         NUM_C205_ALIAS ,
         _c205_opts ,
         _c205_numerics ,
         NULL,
         NULL,
    } ;
