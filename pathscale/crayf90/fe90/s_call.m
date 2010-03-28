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


/* USMID:  "\n@(#)5.0_pl/macros/s_call.m	5.1	04/29/99 21:22:31\n" */


/*****************\
|* MISCELLANEOUS *|
\*****************/


# define MAX_NUM_ACT_TYPES		23
# define MAX_NUM_DUMMY_TYPES		9

#ifdef KEY /* Aid debugging */
typedef enum {
  ERROR_ASSOC =			0,
  PASS_ADDRESS =		1,
  PASS_ADDRESS_FROM_DV =	2,
  PASS_DV =			3,
  PASS_DV_COPY =		4,
  COPY_IN =			5,
  COPY_IN_COPY_OUT =		6,
  MAKE_DV =			7,
  COPY_IN_MAKE_DV =		8,
  MAKE_NEW_DV =			9,
  PASS_SECTION_ADDRESS =	10,
  CHECK_CONTIG_FLAG =		11,
  COPY_INOUT_MAKE_DV =		12
  } association_t;
#else /* KEY */
# define ERROR_ASSOC			0
# define PASS_ADDRESS			1
# define PASS_ADDRESS_FROM_DV		2
# define PASS_DV			3
# define PASS_DV_COPY			4
# define COPY_IN			5
# define COPY_IN_COPY_OUT		6
# define MAKE_DV			7
# define COPY_IN_MAKE_DV		8
# define MAKE_NEW_DV			9
# define PASS_SECTION_ADDRESS		10
# define CHECK_CONTIG_FLAG		11
# define COPY_INOUT_MAKE_DV             12
#endif /* KEY */

/********************\
|* SIZES AND LIMITS *|
\********************/

# define ARGCHCK_SIZE_IDX		3
# define ARGCHCK_CHAR_LEN_IDX		4

#ifdef _TARGET64
# define NUM_TARGET_ARGCHCK_DESC_WORDS	12
# define ARGCHCK_STRUCT_TBL_IDX		12
# else
# define NUM_TARGET_ARGCHCK_DESC_WORDS	16
# define ARGCHCK_STRUCT_TBL_IDX		16
# endif

/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/



/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/
