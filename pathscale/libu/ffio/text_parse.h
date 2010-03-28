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


/* USMID @(#) libu/ffio/text_parse.h	92.1	10/07/99 22:14:06 */

/*  TEXT LAYER ASSIGN PARSING DEFINITION */
#include "txtio.h"
#define NUM_TEXT_ALIAS    0

struct LAYER_OPTS _text_opts[] = {
   CLASS_TEXT, _STR1M,  _INFO1_STR1(TEXT_NL)    ,  0, 0, _STR1M, 0, 0,  "nl" ,
   CLASS_TEXT, _STR1M,  _INFO1_STR1(TEXT_NL_WEOF), 0, 0, _STR1M, 0, 0,  "eof" ,
   CLASS_TEXT, _STR1M,  _INFO1_STR1(TEXT_205)   ,  0, 0, _STR1M, 0, 0, "c205",
   CLASS_TEXT, _STR1M,  _INFO1_STR1(TEXT_205)   ,  0, 0, _STR1M, 0, 0, "205",
   CLASS_TEXT, _STR1M,  _INFO1_STR1(TEXT_CTSS)  ,  0, 0, _STR1M, 0, 0, "ctss"
} ;

struct LAYER_NUMERICS _text_numerics[]  = {
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
   1, 1, 'n' , 0 ,     0xff , 0 ,  "newline"     /*  new line character */   ,
   1, 1, 'n' , 0 ,  0xfffff , 0 ,  "bufsize"     /*  buffer size */
#else
   1, 0, 'n' , 0 ,     0xff , 0 ,  "newline"     /*  new line character */   ,
   1, 0, 'n' , 0 ,  0xfffff , 0 ,  "bufsize"     /*  buffer size */
#endif
} ;

#define NUM_TEXT_OPTS (sizeof(_text_opts)/sizeof(struct LAYER_OPTS))
#define NUM_TEXT_NUMERICS (sizeof(_text_numerics)/sizeof(struct LAYER_NUMERICS))

struct LAYER_DATA _text_data =
    {
         CLASS_TEXT ,
         FLD_EXT_TYPE,
         "text" ,
         ".nl" ,
         0,
         0,
         NUM_TEXT_OPTS,
         1 ,
         NUM_TEXT_NUMERICS,
         NUM_TEXT_ALIAS ,
         _text_opts ,
         _text_numerics ,
         NULL,
         NULL
    } ;

