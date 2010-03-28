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


/* USMID @(#) libu/ffio/cca_parse.h	92.1	10/14/99 15:20:31 */

/*  CACHEA LAYER ASSIGN PARSING DEFINITION */
#include "ccaio.h"
#define NUM_CCA_ALIAS    0

struct LAYER_OPTS _cca_opts[] = {
   CLASS_CACHEA, _STR1M, _INFO1_STR1(TR_CCA_MEM), 0, 0, _STR1M, 0, 0, "mem",
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
   CLASS_CACHEA, _STR1M, _INFO1_STR1(TR_CCA_SDS), 0, 0, _STR1M, 0, 0, "sds",
   CLASS_CACHEA, _STR1M, _INFO1_STR1(TR_CCA_SDS), 0, 0, _STR1M, 0, 0, "ssd" 
#endif
} ;

struct LAYER_NUMERICS _cca_numerics[]  = {
   { 1, 0, 'b' , 1 ,   0xfffff  ,  0 ,  "page_size"   } , /*  number of blocks for buffer */
   { 1, 0, 'n' , 1 ,   0xfffff  ,  0 ,  "num_pages"   } , /*  number of blocks for buffer */
   { 1, 0, 'n' , 0 ,       256  ,  0 ,  "max_lead"    } , /*  read ahead maximum */
   { 1, 1, 'n' , 0 ,       15  ,   0 ,  "shared_cache"} , /*  shared cache number */
                     } ;
#define NUM_CCA_OPTS     (sizeof(_cca_opts)/sizeof(struct LAYER_OPTS))
#define NUM_CCA_NUMERICS (sizeof(_cca_numerics)/sizeof(struct LAYER_NUMERICS))

struct LAYER_DATA _cca_data =
    {
         CLASS_CACHEA ,
         FLD_EXT_TYPE,
         "cachea",
         "" ,
         0,
         0,
         NUM_CCA_OPTS,
         1 ,
         NUM_CCA_NUMERICS,
         NUM_CCA_ALIAS ,
         _cca_opts ,
         _cca_numerics ,
         NULL,
         NULL
    } ;
