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


#ifndef wn2f_pragma_INCLUDED
#define wn2f_pragma_INCLUDED
/* ====================================================================
 * ====================================================================
 *
 * Module: wn2f_pragma.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *    12-Aug-95 - Original Version
 *
 * Description:
 *
 *    WN2F_pragma:
 *       Translates a pragma present in a body of statements.
 *
 *    WN2F_pragma_list_begin:
 *       Translates the list of pragmas associated with PU or a
 *       region.
 *
 *    WN2F_pragma_list_end:
 *       Completes the translation initiated with a call to 
 *       WN2F_pragma_list_begin(), after the body of the PU or region
 *       has been translated.
 *
 * ====================================================================
 * ====================================================================
 */

extern BOOL
   WN2F_Skip_Pragma_Stmt(WN *wn);

extern WN2F_STATUS 
   WN2F_pragma(TOKEN_BUFFER tokens, WN *wn, WN2F_CONTEXT context);


WN2F_STATUS 
   WN2F_pragma_list_begin(TOKEN_BUFFER tokens,
                          WN          *first_pragma,
                          WN2F_CONTEXT context);

WN2F_STATUS 
   WN2F_pragma_list_end(TOKEN_BUFFER tokens, 
                        WN          *first_pragma,
                        WN2F_CONTEXT context);

BOOL
Ignore_Synchronized_Construct(WN          *construct_pragma,  
			      WN2F_CONTEXT context);

#endif /* wn2f_pragma_INCLUDED */


