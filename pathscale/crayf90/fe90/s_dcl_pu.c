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



static char USMID[] = "\n@(#)5.0_pl/sources/s_dcl_pu.c	5.2	06/16/99 10:02:23\n";


# include "defines.h"		/* Machine dependent ifdefs */

# include "host.m"		/* Host machine dependent macros.*/
# include "host.h"		/* Host machine dependent header.*/
# include "target.m"		/* Target machine dependent macros.*/
# include "target.h"		/* Target machine dependent header.*/

# include "globals.m"
# include "tokens.m"
# include "sytb.m"
# include "s_globals.m"
# include "debug.m"

# include "globals.h"
# include "tokens.h"
# include "sytb.h"
# include "s_globals.h"


/*****************************************************************\
|* Function prototypes of static functions declared in this file *|
\*****************************************************************/



/******************************************************************************\
|*									      *|
|* Description:								      *|
|*      BNF     - BLOCK DATA [block-data-name]                                *|
|*									      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	NONE								      *|
|*									      *|
\******************************************************************************/

void blockdata_stmt_semantics (void)

{
   TRACE (Func_Entry, "blockdata_stmt_semantics", NULL);

   TRACE (Func_Exit, "blockdata_stmt_semantics", NULL);

   return;

}  /* blockdata_stmt_semantics */


/******************************************************************************\
|*									      *|
|* Description:								      *|
|*									      *|
|*      BNF     - ENTRY entry-name [([dummy-arg-list])[RESULT (result-name)]] *|
|*              - dummy-arg     =>      dummy-arg-name or *		      *|
|*									      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	NONE								      *|
|*									      *|
\******************************************************************************/

void entry_stmt_semantics (void)

{
   TRACE (Func_Entry, "entry_stmt_semantics", NULL);

   if (cdir_switches.parallel_region) {
      /* a entry stmt is illegal within a parallel region */
      PRINTMSG(stmt_start_line, 549, Error, stmt_start_col, "ENTRY");
   }

   ATP_ALIGN(IR_IDX_L(SH_IR_IDX(curr_stmt_sh_idx)))	= cdir_switches.align;
   cdir_switches.align					= FALSE;

   TRACE (Func_Exit, "entry_stmt_semantics", NULL);

   return;

}  /* entry_stmt_semantics */


/******************************************************************************\
|*									      *|
|* Description:								      *|
|*      BNF     - [ prefix ] FUNCTION function-name                           *|
|*                                   ( [ dummy-arg-name-list ] )              *|
|*                                   [ RESULT ( result-name ) ]               *|
|*                                                                            *|
|*      prefix  -    type-spec [ RECURSIVE ]                                  *|
|*                or RECURSIVE [ type-spec ]				      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	NONE								      *|
|*									      *|
\******************************************************************************/

void function_stmt_semantics (void)

{
   TRACE (Func_Entry, "function_stmt_semantics", NULL);

   TRACE (Func_Exit, "function_stmt_semantics", NULL);

   return;

}  /* function_stmt_semantics */


/******************************************************************************\
|*									      *|
|* Description:								      *|
|*      BNF     -  MODULE module-name                                         *|
|*              or MODULE PROCEDURE procedure-name-list                       *|
|*									      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	TRUE if this is a MODULE or MODULE PROCECURE statement, else FALSE.   *|
|*									      *|
\******************************************************************************/

void module_stmt_semantics (void)

{
   TRACE (Func_Entry, "module_stmt_semantics", NULL);


   TRACE (Func_Exit, "module_stmt_semantics", NULL);

   return;

}  /* module_stmt_semantics */


/******************************************************************************\
|*									      *|
|* Description:								      *|
|*	This function handles the following syntax:                           *|
|*	   program-stmt 	=> PROGRAM program-name [(string of chars)]   *|
|*									      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	NONE								      *|
|*									      *|
\******************************************************************************/

void program_stmt_semantics (void)

{
   TRACE (Func_Entry, "program_stmt_semantics", NULL);

   TRACE (Func_Exit, "program_stmt_semantics", NULL);

   return;

}  /* program_stmt_semantics */


/******************************************************************************\
|*									      *|
|* Description:								      *|
|*	subroutine-stmt		=> [ RECURSIVE ] SUBROUTINE subroutine-name   *|
|*				                   [ ( [ dummy-arg-list ] ) ] *|
|*	dummy-arg 		=> dummy-arg-name			      *|
|*				   or *					      *|
|*									      *|
|* Input parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Output parameters:							      *|
|*	NONE								      *|
|*									      *|
|* Returns:								      *|
|*	NONE								      *|
|*									      *|
\******************************************************************************/

void subroutine_stmt_semantics (void)

{

   TRACE (Func_Entry, "subroutine_stmt_semantics", NULL);

   TRACE (Func_Exit, "subroutine_stmt_semantics", NULL);
   
   return;

}  /* subroutine_stmt_semantics */
