/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* USMID @(#) libu/ffio/cch_parse.h	92.1	10/07/99 15:40:23 */

/*  CACHE LAYER ASSIGN PARSING DEFINITION */

#include "cchio.h"
#define NUM_CACHE_ALIAS    0

struct LAYER_OPTS _cch_opts[] = {
   CLASS_CACHE, _STR1M, _INFO1_STR1(TR_CCH_MEM), 0, 0, _STR1M, 0, 0, "mem" ,
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
   CLASS_CACHE, _STR1M, _INFO1_STR1(TR_CCH_SDS), 0, 0, _STR1M, 0, 0, "sds",
   CLASS_CACHE, _STR1M, _INFO1_STR1(TR_CCH_SDS), 0, 0, _STR1M, 0, 0, "ssd"
#endif
} ;

struct LAYER_NUMERICS _cch_numerics[]  = {
   1, 0, 'b', 1 , 0xfffff,  0 ,  "page_size", /*  number of blocks for buffer */   
   1, 0, 'n', 1 , 0xfffff,  0 ,  "num_pages", /*  number of blocks for buffer */
   /* number of blocks at which we bypass buffering */
   1, 1, 'b', 1 , 0xffffffffffffLL,  0 ,  "bypass_size"
} ;

#define NUM_CACHE_OPTS     (sizeof(_cch_opts)/sizeof(struct LAYER_OPTS))
#define NUM_CACHE_NUMERICS (sizeof(_cch_numerics)/sizeof(struct LAYER_NUMERICS))

struct LAYER_DATA _cch_data =
    {
         CLASS_CACHE ,
         FLD_EXT_TYPE,
         "cache",
         "" ,
         0,
         0,
         NUM_CACHE_OPTS,
         1 ,
         NUM_CACHE_NUMERICS,
         NUM_CACHE_ALIAS ,
         _cch_opts ,
         _cch_numerics ,
         NULL,
         NULL
    } ;

