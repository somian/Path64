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


/* USMID @(#) libu/ffio/blx_parse.h	92.0	10/08/98 14:57:41 */

/*  BLX LAYER ASSIGN PARSING DEFINITION */

#include "blxio.h"
#define NUM_BLX_ALIAS    0

struct LAYER_OPTS _blx_opts[] = {
	CLASS_BLX, _STR1M, _INFO1_STR1(BLX_COS) , 0, 0, _STR1M, 0, 0, "cos" ,
	CLASS_BLX, _STR1M, _INFO1_STR1(BLX_CTSS), 0, 0, _STR1M, 0, 0, "ctss" ,
	CLASS_BLX, _STR1M, _INFO1_STR1(BLX_CTSS), 0, 0, _STR1M, 0, 0, "c205" ,
} ;

struct LAYER_NUMERICS _blx_numerics[]  = {
   1, 1, 'n' , 0 , 0xff  , 0 ,  "blxchr" ,
   1, 1, 'n' , 0 , 0xff  , 0 ,  "blnk"
                     } ;
#define NUM_BLX_OPTS     sizeof(_blx_opts)/sizeof(struct LAYER_OPTS)
#define NUM_BLX_NUMERICS sizeof(_blx_numerics)/sizeof(struct LAYER_NUMERICS)

struct LAYER_DATA _blx_data =
    {
	CLASS_BLX	,
         FLD_EXT_TYPE     ,
         "blx",
         ".cos" ,
         0,
         0,
         NUM_BLX_OPTS,
         1 ,
         NUM_BLX_NUMERICS,
         NUM_BLX_ALIAS ,
         _blx_opts ,
         _blx_numerics ,
         NULL,
         NULL
    } ;
struct LAYER_DATA _blankx_data =
    {
	CLASS_BLX	,
         FLD_EXT_TYPE     ,
         "blankx",
         ".cos" ,
         0,
         0,
         NUM_BLX_OPTS,
         1 ,
         NUM_BLX_NUMERICS,
         NUM_BLX_ALIAS ,
         _blx_opts ,
         _blx_numerics ,
         NULL,
         NULL
	};

