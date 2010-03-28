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


#ifndef wn2f_load_store_INCLUDED
#define wn2f_load_store_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: wn2f_load_store.h
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
 * ====================================================================
 * ====================================================================
 */

extern void WN2F_Load_Store_initialize(void);
extern void WN2F_Load_Store_finalize(void);
      
extern WN2F_STATUS 
   WN2F_istore(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_istorex(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_mstore(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_stid(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern WN2F_STATUS 
   WN2F_iload(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_iloadx(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_mload(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_ldid(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS 
   WN2F_lda(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern WN2F_STATUS
   WN2F_array(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);
extern void
   WN2F_array_bounds(TOKEN_BUFFER tokens, 
		     WN *wn, 
		     TY_IDX array_ty,
		     WN2F_CONTEXT context) ;
extern void
   WN2F_String_Argument(TOKEN_BUFFER  tokens,
			WN           *base,
			WN           *length,
			WN2F_CONTEXT  context);


extern BOOL WN2F_Is_Address_Preg(WN * wn ,TY_IDX ptr_ty) ;

#endif /* wn2f_load_store_INCLUDED */

