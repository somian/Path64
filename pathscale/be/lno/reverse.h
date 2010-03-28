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

/** Description: 
***
***   This file contains the description of loop reversal functions provided
***   in the LNO phase of the optimizer.  
***
***   Currently only simple cases are handled, i.e. those for which the loop
***   bounds have the form: 
***
*** 	for (i = exp1; i <= exp2; i++) 
***
***   More complex cases will be added if it is deemed worthwhile. 
***
***   Two external functions are provided: 
***
***	BOOL RV_Is_Legal(WN* wn_loop)
***
***	  Returns TRUE if reversing loop 'wn_loop' is legal, FALSE
***       otherwise. 
***
***	void RV_Reverse_Loop(WN* wn_loop)
*** 
***	  Reverse the loop 'wn_loop'.  We assume that the loop has
***	  already been determined to be reversible.
***
***   One other external function is used for testing only: 
*** 
***	void Reverse_Loops(WN* func_nd)
***
***	  Reverse every loop in the 'func_nd' for which loop reversal
***	  is legal.
***
***	BOOL Do_Loop_Is_Backward(WN* wn_loop)
***
***	  Returns TRUE if every do loop index variable in the body of 
***	  the loop 'wn_loop' is provably negative, FALSE otherwise. 
***
***
**/

extern BOOL RV_Is_Legal(WN* wn_loop);
extern void RV_Reverse_Loop(WN* wn_loop);
extern void Reverse_Loops(WN* func_nd);
extern BOOL Do_Loop_Is_Backward(WN* wn_loop); 
    
