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


/* USMID:  "\n@(#)5.0_pl/macros/s_globals.m	5.2	05/27/99 10:30:26\n" */


/*****************\
|* MISCELLANEOUS *|
\*****************/

/* LCV_CONST_IDX is used by SET_LCV_CONST.				      */

# if _HOST64
# define        LCV_CONST_IDX   2
# else
# define        LCV_CONST_IDX   4
# endif

/********************\
|* SIZES AND LIMITS *|
\********************/



/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/

# define SHAPE_FOLDABLE(OPND)		OPND.flag_1
# define SHAPE_WILL_FOLD_LATER(OPND)	OPND.flag_2


/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/

# define COPY_SHAPE(SHAPE_T, SHAPE_S, RANK)                                    \
                { int   _i;                                                    \
                        for(_i = 0; _i < RANK; _i++) {                         \
                           COPY_OPND(SHAPE_T[_i], SHAPE_S[_i]);                \
                         }                                                     \
                }


/* SET_LCV_CONST is used by the routines in s_cnstrct.c and in s_data.c       */
/* (implied-DO processing) to save away the value of an implied-DO variable   */
/* in an Attr entry.							      */
/* Note - A target constant is stored and retrieved                           */

# define LCV_WORDS(VAR)  (sizeof(VAR) / sizeof(attr_tbl[0].wd[0]))
# ifdef KEY
# define SET_LCV_CONST(ATTR, VALUE, WORDS, VAL_WORDS) {				       \
	 attr_tbl[ATTR].wd[LCV_CONST_IDX] = *(long *) &((VALUE));	       \
	 if (WORDS == 2) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+1);     \
	 }								       \
	 else if (WORDS == 3) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+1);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+2] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+2);     \
	 }								       \
	 else if (WORDS == 4) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+1);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+2] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+2);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+3] = (VAL_WORDS<WORDS)?0:*((long *) &((VALUE))+3);     \
	 }								       \
	 }
# else
# define SET_LCV_CONST(ATTR, VALUE, WORDS) {				       \
	 attr_tbl[ATTR].wd[LCV_CONST_IDX] = *(long *) &((VALUE));	       \
	 if (WORDS == 2) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = *((long *) &((VALUE))+1);     \
	 }								       \
	 else if (WORDS == 3) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = *((long *) &((VALUE))+1);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+2] = *((long *) &((VALUE))+2);     \
	 }								       \
	 else if (WORDS == 4) {						       \
	    attr_tbl[ATTR].wd[LCV_CONST_IDX+1] = *((long *) &((VALUE))+1);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+2] = *((long *) &((VALUE))+2);     \
            attr_tbl[ATTR].wd[LCV_CONST_IDX+3] = *((long *) &((VALUE))+3);     \
	 }								       \
	 }
# endif

# define GET_LCV_CONST(ATTR, VALUE, WORDS) {                                   \
         *(long *) &((VALUE)) = attr_tbl[ATTR].wd[LCV_CONST_IDX];              \
         if (WORDS == 2) {                                                     \
            *((long *) &((VALUE))+1) = attr_tbl[ATTR].wd[LCV_CONST_IDX+1];     \
         }                                                                     \
         else if (WORDS == 3) {                                                \
            *((long *) &((VALUE))+1) = attr_tbl[ATTR].wd[LCV_CONST_IDX+1];     \
            *((long *) &((VALUE))+2) = attr_tbl[ATTR].wd[LCV_CONST_IDX+2];     \
         }                                                                     \
         else if (WORDS == 4) {                                                \
            *((long *) &((VALUE))+1) = attr_tbl[ATTR].wd[LCV_CONST_IDX+1];     \
            *((long *) &((VALUE))+2) = attr_tbl[ATTR].wd[LCV_CONST_IDX+2];     \
            *((long *) &((VALUE))+3) = attr_tbl[ATTR].wd[LCV_CONST_IDX+3];     \
         }                                                                     \
         }
