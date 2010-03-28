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


#ifndef wn2f_io_INCLUDED
#define wn2f_io_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: wn2f_io.h
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
 *    Utilities for translation an IOS node or an IO item within
 *    an IOS.  Note that WN2F_io_prefix_tokens will only return a
 *    valid token buffer once WN2F_CONTEXT_io_stmt(context) is set.
 *
 * ====================================================================
 * ====================================================================
 */

extern void WN2F_Io_initialize(void);
extern void WN2F_Io_finalize(void);

extern TOKEN_BUFFER
   WN2F_io_prefix_tokens(void);

extern WN2F_STATUS 
   WN2F_io(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);

extern BOOL
   WN2F_io_item(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);


#endif /* wn2f_io_INCLUDED */

