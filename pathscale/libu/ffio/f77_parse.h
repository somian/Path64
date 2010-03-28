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


/* USMID @(#) libu/ffio/f77_parse.h	92.1	10/29/99 21:40:31 */

/*  F77 LAYER ASSIGN PARSING DEFINITION */

#include "f77io.h"
#include "common_parse.h"

#define NUM_F77_ALIAS    0

struct LAYER_OPTS _f77_opts[] = {
   CLASS_F77, _STR1M, _INFO1_STR1(TR_UX_MIPS), 0, 0, _STR1M, 0, 0, "mips" ,
   CLASS_F77, _STR1M, _INFO1_STR1(TR_UX_INTEL), 0, 0, _STR1M, 0, 0, "intel"
} ;

/* TODO: 7 Oct 2000: probably not right for little endian */

struct LAYER_NUMERICS _f77_numerics[]  = {
   1, 1, 'n' , 0 , 0x7fffffff , 0 ,  "recsize" ,
   1, 0, 'n' , 0 , 0xfffff  , 0 ,  "bufsize"
} ;

#define NUM_F77_OPTS     (sizeof(_f77_opts)/sizeof(struct LAYER_OPTS))
#define NUM_F77_NUMERICS sizeof (_f77_numerics)/sizeof(struct LAYER_NUMERICS)

struct LAYER_DATA _f77_data =
    {
         CLASS_F77 ,
         FLD_TYPE     ,
         "f77",
         ".intel" ,
         0,
         0,
         NUM_F77_OPTS,
         1 ,
         NUM_F77_NUMERICS,
         NUM_F77_ALIAS ,
         _f77_opts ,
         _f77_numerics ,
         NULL,
         NULL
    } ;

