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


/* USMID:  "\n@(#)5.0_pl/macros/tokens.m	5.1	04/29/99 21:22:31\n" */
 

/*****************\
|* MISCELLANEOUS *|
\*****************/


/********************\
|* SIZES AND LIMITS *|
\********************/


/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/

# define  LA_CH_CLASS			la_ch.ch_class
# define  LA_CH_COLUMN			la_ch.column
# define  LA_CH_LINE			la_ch.line
# define  LA_CH_VALUE			la_ch.value
# define  LA_CH_BUF_IDX			la_ch.stmt_buf_idx
# define  LA_CH_STMT_NUM		la_ch.stmt_num

# define  TOKEN_COLUMN(TOK)		(TOK).column
# define  TOKEN_CONST_TBL_IDX(TOK)	TOKEN_STR_WD(TOK, 0)
# define  TOKEN_KIND_LEN(TOK)		(TOK).kind_len
# define  TOKEN_KIND_STR(TOK)		(TOK).kind_str
# define  TOKEN_LEN(TOK)		(TOK).token_len
# define  TOKEN_LINE(TOK)		(TOK).line
# define  TOKEN_STR(TOK)		(TOK).token_str.string
# define  TOKEN_STR_WD(TOK, WD)		(TOK).token_str.words[WD]
# define  TOKEN_VALUE(TOK)		(TOK).value
# define  TOKEN_ID(TOK)			(TOK).token_str
# define  TOKEN_ERR(TOK)		(TOK).token_err
# define  TOKEN_BUF_IDX(TOK)		(TOK).stmt_buf_idx
# define  TOKEN_STMT_NUM(TOK)		(TOK).stmt_num


/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/


/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/
# define LA_CH_TO_ERR_STR(STR,THE_LA)                                          \
         STR =  (THE_LA.ch_class == Ch_Class_EOS) ? "EOS" : &THE_LA.value
