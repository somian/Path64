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


/* USMID @(#) libu/ffio/bmx_parse.h	92.0	10/08/98 14:57:41 */

/*  BMX LAYER ASSIGN PARSING DEFINITION */

#include "common_parse.h"

#define NUM_BMX_OPTS     0
#define NUM_BMX_ALIAS    0


struct LAYER_DATA _bmx_data =
    {
         CLASS_BMX ,
         FLD_EXT_TYPE,
         "bmx",
         "" ,
         0,
         0,
         NUM_BMX_OPTS,
         1 ,
         NUM_NBUF_NUMERICS,
         NUM_BMX_ALIAS ,
         NULL ,
         _nbuf_numerics ,
         NULL,
         NULL
    } ;
struct LAYER_DATA _tape_data =
    {
         CLASS_BMX ,
         FLD_EXT_TYPE,
         "tape",
         "" ,
         0,
         0,
         NUM_BMX_OPTS,
         1 ,
         NUM_NBUF_NUMERICS,
         NUM_BMX_ALIAS ,
         NULL ,
         _nbuf_numerics ,
         NULL,
         NULL
	};
