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


 #ifndef init2f_INCLUDED
#define init2f_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: init2f.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  17-June-94 - Original Version
 *
 * Description:
 *
 *    INITO2F_translate:
 *       Appends a DATA statement for the ST associated with the given
 *       INITO list.  In the process, some equivalence specifications
 *       may be prepended to the Data_Stmt_Tokens.  Typical is as
 *       follows:
 *
 *           1) Upon entering a new PU block: Initialize Data_Stmt_Tokens
 *
 *           2) Upon declaring a variable: If it is initialized append
 *              the initializer to Data_Stmt_Tokens with a call to
 *	        INITO2F_translate(Data_Stmt_Tokens, inito).
 *
 *           3) Upon exiting a PU block: Append the contents of 
 *              Data_Stmt_Tokens to the PU tokens, and reclaim the
 *              Data_Stmt_Tokens TOKEN_BUFFER.
 *
 * ====================================================================
 * ====================================================================
 */

extern TOKEN_BUFFER Data_Stmt_Tokens; 

extern void INITO2F_translate(TOKEN_BUFFER tokens, INITO_IDX inito);

#endif /* init2f_INCLUDED */
