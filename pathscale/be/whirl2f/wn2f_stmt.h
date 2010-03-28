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


#ifndef wn2f_stmt_INCLUDED
#define wn2f_stmt_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: wn2f_stmt.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *    13-Apr-95 - Original Version
 *
 * Description:
 *
 *    WN2F_Stmt_Initialize:  
 *       First thing to do in the handler for a func_entry, which
 *       will setup the state for statement translation.
 *
 *    WN2F_Stmt_Finalize:
 *       Last thing we need to do.
 *
 * ====================================================================
 * ====================================================================
 */

extern void WN2F_Stmt_initialize(void);
extern void WN2F_Stmt_finalize(void);

/* Some statements can be skipped, based on previous analysis of
 * call and return sites (e.g. storing to/from return registers).
 */
extern BOOL WN2F_Skip_Stmt(WN *stmt);

extern void WN2F_Append_Purple_Funcinfo(TOKEN_BUFFER tokens);

extern WN2F_STATUS 
   WN2F_block(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_region(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_compgoto(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_do_loop(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_implied_do(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);


extern WN2F_STATUS 
   WN2F_do_while(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_while_do(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_if(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_goto(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_agoto(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_condbr(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_return(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_return_val(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_label(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_istore(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_intrinsic_call(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_call(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_prefetch(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_eval(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern void
   WN2F_Append_Block_Data(TOKEN_BUFFER t);

#endif /* wn2f_stmt_INCLUDED */


