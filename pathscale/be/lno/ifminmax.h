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


// -*-C++-*-

/**
***  Exported types and functions:
***
***    void If_MinMax(WN* func_nd)
***
***	Convert expressions of the form: 
***	  if (expr1 .relop. expr2) then
***	    result = expr1
***	  else 
***	    result = expr2
***	  end if 
***	to expressions of the form:
***	  result = min(expr1, expr2)
***	or
***	  result = max(expr1, expr2)
***	where .relop. is one of .LT., .GT., .LE., and .GE.
***
***	Attempt sinking converted expressions into loop bounds where
***	  appropriate.
**/

#ifndef ifminmax_INCLUDED
#define ifminmax_INCLUDED "cache_model.h"

extern void If_MinMax(WN* func_nd); 

#endif // ifminmax_INCLUDED
