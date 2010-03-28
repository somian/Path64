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


#ifndef util_INCLUDED
#define util_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif



#ifdef _KEEP_RCS_ID
static char *util_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* Generate a call to an external command:
 * Returns 0 on success, an error code on failure.
 * An error code < 0400 is a Unix error code from errno.h.
 * An error code > 0400 is 0400 plus the signal which killed child.
 */
extern INT Execute (
  char *cmd,		/* The command to execute */
  char **argv,		/* Argument list */
  char *stdoutfile,	/* stdout for new process to use or NULL */
  BOOL echo		/* Echo command line? */
);

/* Get the value of an environment variable: */
extern char *Get_Environment_Value (
  char *name,	/* The name of the environment variable */
  char **envp,	/* Array of environment variables + NULL */
  char *def	/* Default to return if not found */
);

/* Check an integer value against bounds: */
extern INT Check_Range (
  INT val,	/* Check this value */
  INT lbound,	/* ... against this lower bound */
  INT ubound,	/* ... and this upper bound, */
  INT def	/* ... with this default. */
);		/* Returns val if in range, def if out of range. */

/* Print tabs and spaces to indent given number of characters: */
extern void Indent (
  FILE *f,	/* File to indent */
  INT16 indent	/* Number of spaces to indent */
);

/* Mathematically correct integer modulus function: */
INT Mod(
  INT i,
  INT j
);

/* Min & Max functions.  These really have to be functions because
 * they evaluate their arguments an indeterminate number of times.  (1
 * or 2).
 */
extern INT Min(
  INT i,
  INT j
);

extern INT Max(
  INT i,
  INT j
);

/* Count set bits */
extern const mUINT8 UINT8_pop_count[256];
extern TARG_INT
TARG_INT_Pop_Count(
  TARG_INT x
);

/* Find leftmost set bit */
/*	extern const mUINT8 UINT8_most_sig_one[256];	*/
extern TARG_INT
TARG_INT_Most_Sig_One(
  TARG_INT x
);

/* Find rightmost set bit. */
extern const mUINT8 UINT8_least_sig_one[256];
extern TARG_INT
TARG_INT_Least_Sig_One(
  TARG_INT x
);

extern INT32 nearest_power_of_two(INT32 n);

/* Is there a string of ones from bit ub to lb ([63 .. 0])	*/
extern BOOL Immediate_Has_All_Ones(INT64 imm, INT32 ub, INT32 lb);


#ifdef __cplusplus
}
#endif
#endif /* util_INCLUDED */
