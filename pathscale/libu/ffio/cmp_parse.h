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


/* USMID @(#) libu/ffio/cmp_parse.h	92.1	06/10/99 15:02:15 */

/*  CMP LAYER ASSIGN PARSING DEFINITION */

#define NUM_CMP_OPTS     0
#define NUM_CMP_ALIAS    0

struct LAYER_NUMERICS _cmp_numerics[]  = {
   0, 0, 'n' , 0x200  , 0x800000 , 0x2000 ,  "segSz" ,
   0, 0, 'n' , 1 , 50 , 10 ,  "cacheSz",
   0, 0, 'n' , 0 , 4  , 0 ,  "debugLvl"
} ;

#define NUM_CMP_NUMERICS sizeof(_cmp_numerics)/sizeof(struct LAYER_NUMERICS)

struct LAYER_DATA _cmp_data =
    {
	CLASS_CMP,
	FLD_EXT_TYPE,
	"cmp",
	"" ,
	0,
	0,
	NUM_CMP_OPTS,
	1,
	NUM_CMP_NUMERICS,
	NUM_CMP_ALIAS,
	NULL,
	_cmp_numerics,
	NULL,
	NULL
    };
