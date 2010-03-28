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



#ifndef f90_lower_INCLUDED
#define f90_lower_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* This is the main external interface to the F90 lowerer 
 *
 *  WN * F90_Lower (WN *pu);
 *
 * Lowers the program unit in *pu. Returns a lowered program unit
 * (most likely, it will be the same pointer passed in, with a lot of changed subtrees,
 * but not necessarily).
 *
 */
extern WN * F90_Lower (PU_Info* pu_info, WN *pu);


/* Some enumerated types needed by the lowerer; these don't need to be 
   generally visible, but they are needed in two files */
#ifdef F90_LOWER_INTERNAL

/*
 * COPY_FLAG indicates what sort of temp copies need to be done for 
 * this statement.
 */
typedef enum {
   COPY_NONE=0,
   COPY_LEFT=1,
   COPY_RIGHT=2,
   COPY_BOTH=3  /* Must be COPY_LEFT & COPY_RIGHT */
} COPY_FLAG_T;

/*
 * DIR_FLAG indicates which direction a DO loop should be run. This is also used for dependence
 * analysis.
 */
typedef enum {
   DIR_DONTCARE=0,
   DIR_POSITIVE=1,
   DIR_NEGATIVE=2,
   DIR_ZERO=3,
   DIR_UNKNOWN=4
} DIR_FLAG;

typedef enum {
   DEP_UNKNOWN = 0,
   DEP_INDEPENDENT = 1,
   DEP_IDENTICAL = 2,
   DEP_REMOVABLE = 3
} DEP_SUMMARY;

/* This is the interface to the c++ routine for doing triplet dependence
 * analysis. It's in c++ becuase it makes use of LNO's SYSTEM_OF_EQUATIONS
 * class
 */
extern DIR_FLAG F90_Lower_Analyze_Triplet (INT64 l, INT64 s1, INT64 s2, 
					   INT64 e, BOOL e_present, MEM_POOL *mp);

#endif

#ifdef __cplusplus
}
#endif
#endif /* wn_lower_INCLUDED */
