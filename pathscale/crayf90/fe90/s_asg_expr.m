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


/* USMID:  "\n@(#)5.0_pl/macros/s_asg_expr.m	5.1	04/29/99 21:22:31\n" */


/*****************\
|* MISCELLANEOUS *|
\*****************/

# ifdef _TARGET64
#   define	INT_WORD	Integer_8
# else
#   define	INT_WORD	Integer_4
# endif

# ifdef _TARGET64
#   define	MACH_SPEC	TRUE , Typeless_8
# else
#   define	MACH_SPEC	FALSE, Err_Res
# endif

# define EQUAL_OP       0
# define NOT_EQUAL      1
# define LESS_THAN	0
# define LESS_EQ	1
# define GREATER_THAN	2
# define GREATER_EQ	3


/********************\
|* SIZES AND LIMITS *|
\********************/


/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/


# define UN_PLUS_TYPE(IDX)                                                     \
	un_plus_tbl[IDX].type

# define UN_PLUS_EXTN(IDX)                                                     \
	un_plus_tbl[IDX].ext 


# define NOT_TYPE(IDX)                                                         \
	not_tbl[IDX].type

# define NOT_EXTN(IDX)                                                         \
	not_tbl[IDX].ext 


# define BIN_SUB_TYPE(IDXL, IDXR)                                              \
	bin_sub_tbl[IDXL][IDXR].type

# define BIN_SUB_EXTN(IDXL, IDXR)                                              \
	bin_sub_tbl[IDXL][IDXR].ext 

# define BIN_ADD_TYPE(IDXL, IDXR)                                              \
	bin_add_tbl[IDXL][IDXR].type

# define BIN_ADD_EXTN(IDXL, IDXR)                                              \
	bin_add_tbl[IDXL][IDXR].ext 


# define MULT_DIV_TYPE(IDXL, IDXR)                                             \
	mult_div_tbl[IDXL][IDXR].type

# define MULT_DIV_EXTN(IDXL, IDXR)                                             \
	mult_div_tbl[IDXL][IDXR].ext 


# define POWER_TYPE(IDXL, IDXR)                                                \
	power_tbl[IDXL][IDXR].type

# define POWER_EXTN(IDXL, IDXR)                                                \
	power_tbl[IDXL][IDXR].ext 


# define EQ_NE_TYPE(IDXL, IDXR)                                                \
	eq_ne_tbl[IDXL][IDXR].type

# define EQ_NE_EXTN(IDXL, IDXR)                                                \
	eq_ne_tbl[IDXL][IDXR].ext 

#ifdef KEY /* Bug 5710 */
# define EQ_NE_ON_LOGICAL(IDXL, IDXR)                                         \
	eq_ne_on_logical_tbl[IDXL][IDXR].type
#endif /* KEY Bug 5710 */

# define LG_TYPE(IDXL, IDXR)                                                   \
	lg_tbl[IDXL][IDXR].type

# define LG_EXTN(IDXL, IDXR)                                                   \
	lg_tbl[IDXL][IDXR].ext 


# define GT_LT_TYPE(IDXL, IDXR)                                                \
	gt_lt_tbl[IDXL][IDXR].type

# define GT_LT_EXTN(IDXL, IDXR)                                                \
	gt_lt_tbl[IDXL][IDXR].ext 


# define AND_OR_TYPE(IDXL, IDXR)                                               \
	and_or_tbl[IDXL][IDXR].type

# define AND_OR_EXTN(IDXL, IDXR)                                               \
	and_or_tbl[IDXL][IDXR].ext 

# define ASG_TYPE(IDXL, IDXR)                                                  \
	asg_tbl[IDXL][IDXR].type

# define ASG_EXTN(IDXL, IDXR)                                                  \
	asg_tbl[IDXL][IDXR].ext 

/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/

