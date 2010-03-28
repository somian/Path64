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


/* USMID:  "\n@(#)5.0_pl/macros/fecif.m	5.1	04/29/99 21:22:31\n" */


/******************************************************************************/
/*                                                                            */
/*                    Misc attribute bit masks                                */
/*                                                                            */
/******************************************************************************/

/* The following bit masks are used in the Entry Information record.          */

# define CIF_PGM_DEFINITION		00000001      /*   0000 0001	      */
# define CIF_PGM_IN_INTERFACE		00000002      /*   0000 0002	      */
# define CIF_PGM_USE_ASSOCIATED         00000004      /*   0000 0004          */
# define CIF_PGM_REFERENCE	        00000010      /*   0000 0008	      */
# define CIF_PGM_OPTIONAL		00000020      /*   0000 0010	      */
# define CIF_PGM_PRIVATE		00000040      /*   0000 0020	      */
# define CIF_PGM_RECURSIVE		00000100      /*   0000 0040	      */


/* The following bit masks are used in the Object Information record.	      */

# define CIF_IMPLICITLY_TYPED		00000001      /*   0000 0001	      */
# define CIF_CRI_POINTEE		00000002      /*   0000 0002	      */
# define CIF_DEFAULT_TYPED		00000004      /*   0000 0004	      */
# define CIF_STAR_TYPED			00000010      /*   0000 0008	      */
# define CIF_KIND_TYPED			00000020      /*   0000 0010	      */
# define CIF_SAVED			00000040      /*   0000 0020	      */
# define CIF_DATA_INIT			00000100      /*   0000 0040	      */
# define CIF_EQUIVALENCED		00000200      /*   0000 0080	      */
# define CIF_DARG_IN_BND		00000400      /*   0000 0100          */
/*	 available for future use	00001000           0000 0200          */
/*	 available for future use	00002000           0000 0400          */
# define CIF_ALLOCATABLE		00004000      /*   0000 0800	      */
# define CIF_INTENT_IN			00010000      /*   0000 1000	      */
# define CIF_INTENT_OUT			00020000      /*   0000 2000	      */
# define CIF_INTENT_INOUT		00040000      /*   0000 4000	      */
# define CIF_OPTIONAL			00100000      /*   0000 8000	      */
# define CIF_POINTER			00200000      /*   0001 0000	      */
# define CIF_PRIVATE			00400000      /*   0002 0000	      */
# define CIF_TARGET			01000000      /*   0004 0000	      */
# define CIF_RENAMED			02000000      /*   0008 0000	      */

/* The following bit masks are used in the Scope Information record.	      */

# define CIF_SCP_IMPL_NONE		00000001      /*   0000 0001	      */
# define CIF_SCP_DOES_IO		00000002      /*   0000 0002	      */
# define CIF_SCP_HAS_CALLS		00000004      /*   0000 0004	      */

/* The following is used for the Use Module Record.                 	      */

# define CIF_USE_MODULE_DIRECT		1
# define CIF_USE_MODULE_INDIRECT	0


/******************************************************************************/
/*                                                                            */
/*                      Miscellaneous Definitions                             */
/*                                                                            */
/******************************************************************************/

# define ASSUMED_SIZE_CHAR	'*'
# define FIXED_FORM       	'0'
# define VAR_LEN_CHAR		'E'


/********************\
|* SIZES AND LIMITS *|
\********************/

#define ARG_BUF_SIZE		(EXPANDED_MSG_SIZE - ORIG_MSG_SIZE)


/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/



/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/

# define NEXT_DERIVED_TYPE_ID  cif_derived_type_id++

# define NEXT_FILE_ID  cif_file_id++

# define NEXT_SCOPE_ID  cif_symbol_or_scope_id++

# define NEXT_SYMBOL_ID  cif_symbol_or_scope_id++
