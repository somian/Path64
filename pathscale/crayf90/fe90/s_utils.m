/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


/* USMID:  "\n@(#)5.0_pl/macros/s_utils.m	5.1	04/29/99 21:22:31\n" */


/*****************\
|* MISCELLANEOUS *|
\*****************/

/* dope vector type code values */

# define DV_TYPELESS				1
# define DV_INTEGER				2
# define DV_REAL				3
# define DV_COMPLEX				4
# define DV_LOGICAL				5
# define DV_ASCII_CHAR				6
# define DV_ASCII_CHAR_SEQUENCE_STRUCT		7
# define DV_STRUCT				8
# define DV_BIT					9
# define DV_2_BYTE_CHAR				10
# define DV_2_BYTE_CHAR_SEQUENCE_STRUCT		11
# define DV_4_BYTE_CHAR				12
# define DV_4_BYTE_CHAR_SEQUENCE_STRUCT		13

# define DV_DEFAULT_TYPED			0
# define DV_KIND_TYPED				1
# define DV_STAR_TYPED				2
# define DV_KIND_CONST				3
# define DV_KIND_DOUBLE				4

# if defined(_BITFIELD_RIGHT_TO_LEFT)         /* Most Intel x86 based  */
                                              /* platforms use R-to-L  */
#    define DV_TYPE_SHIFT			0
#    define DV_DP_SHIFT				8
#    define DV_KIND_STAR_SHIFT			8+1
#    define DV_INT_LEN_SHIFT			8+1+3
#    define DV_DEC_LEN_SHIFT			8+1+3+12
# else
#    define DV_TYPE_SHIFT			24
#    define DV_DP_SHIFT				23
#    define DV_KIND_STAR_SHIFT			20
#    define DV_INT_LEN_SHIFT			8
#    define DV_DEC_LEN_SHIFT			0
# endif

/********************\
|* SIZES AND LIMITS *|
\********************/

# define CALL_LIST_TBL_INC		256



/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/



/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/
