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


/* USMID @(#) libu/ffio/global_parse.h	92.0	10/08/98 14:57:41 */

/*  GLOBAL LAYER ASSIGN PARSING DEFINITION */

#include "globio.h"
#define NUM_GLOBAL_ALIAS    0

struct LAYER_OPTS _global_opts[] = {
 CLASS_GLOBAL, _STR1M, _INFO1_STR1(TR_GLOB_PRIVPOS),   0, 0, _STR1M, 0, 0, "privpos" ,
 CLASS_GLOBAL, _STR1M, _INFO1_STR1(TR_GLOB_GLOBPOS),   0, 0, _STR1M, 0, 0, "globpos" ,
} ;

struct LAYER_NUMERICS _global_numerics[]  = {

   /*  number of blocks for cache page */
   { 1, 0, 'b' , 1 , 0xffffffff, 0 ,  "page_size" }, 

   /*  number of pages per PE */
   { 1, 0, 'n' , 1 , 0xffffffff, 0 ,  "num_pages" },
} ;

#define NUM_GLOBAL_NUMERICS (sizeof(_global_numerics)/sizeof(struct LAYER_NUMERICS))

#define NUM_GLOBAL_OPTS     (sizeof(_global_opts)/sizeof(struct LAYER_OPTS))

struct LAYER_DATA _global_data =
    {
         CLASS_GLOBAL ,
         FLD_TYPE,
         "global",
         "" ,
         0,
         0,
         NUM_GLOBAL_OPTS,
         1 ,
         NUM_GLOBAL_NUMERICS,
         NUM_GLOBAL_ALIAS ,
         _global_opts ,
	 _global_numerics,
         NULL,
         NULL
    } ;

