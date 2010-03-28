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


/* USMID @(#) libu/ffio/cdc_parse.h	92.0	10/08/98 14:57:41 */

/*  CDC LAYER ASSIGN PARSING DEFINITION */

#define NUM_CDC_NUMERICS 0
#define NUM_CDC_ALIAS    0

#include "cdcio.h"
struct LAYER_OPTS _cdc_opts[] = {
   CLASS_CDC, _STR1M,  _INFO1_STR1(TR_CDC_CZ),      0, 0, _STR1M, 0, 0, "cz" ,
   CLASS_CDC, _STR1M,  _INFO1_STR1(TR_CDC_IW),      0, 0, _STR1M, 0, 0, "iw" ,
   CLASS_CDC, _STR1M,  _INFO1_STR1(TR_CDC_CW),      0, 0, _STR1M, 0, 0, "cw" ,
   CLASS_CDC, _STR1M,  _INFO1_STR1(TR_CDC_CS),      0, 0, _STR1M, 0, 0, "cs" ,
   CLASS_CDC, _STR2M,  _INFO1_STR2(TR_CDC_BT_DISK), 0, 0, _STR2M, 0, 0, "disk" ,
   CLASS_CDC, _STR2M,  _INFO1_STR2(TR_CDC_BT_I),    0, 0, _STR2M, 0, 0, "i" ,
   CLASS_CDC, _STR2M,  _INFO1_STR2(TR_CDC_BT_SI),   0, 0, _STR2M, 0, 0, "si"
} ;

#define NUM_CDC_OPTS     (sizeof(_cdc_opts)/sizeof(struct LAYER_OPTS))

struct LAYER_DATA _cdc_data =
    {
         CLASS_CDC ,
         NO_TYPE,
         "cdc",
         ".disk" ,
         _STR1M,
         0,
         NUM_CDC_OPTS,
         1,
         NUM_CDC_NUMERICS,
         NUM_CDC_ALIAS,
         _cdc_opts,
         NULL,
         NULL,
         NULL
    } ;

