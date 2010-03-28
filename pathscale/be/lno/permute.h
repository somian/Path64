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
***     Description:  This file contains functions used to do lego and mp 
***		      permutations. 
***
***	BOOL SNL_Legal_Permutation(WN* outer_loop,
***                                  WN* inner_loop,
***                                  INT permutation[],
***                                  INT nloops)
***
***	    Returns TRUE if the SNL defined between loops 'outer_loop'
***	    and 'inner_loop' which consists of 'nloops' loops can be legally
***	    permuted with specified 'permutation'.  Returns FALSE otherwise.
***	    This test is more conservative than the one used in Phase 2
***	    LNO analysis.  We may wish to extend it later.
***
***	INT Permutation_Last(INT first,
***			     INT permutation[],
***                          INT nloops)
***
***	    Returns the last index in the permutation subsequence in
***	    'permutation' which begins with 'first'. (For example, see
***	    permute.cxx) 
***
***     void Mp_Compress(WN* wn_root)
***
***	    Compress the regions of nested doacrosses into single MP
***	    regions.
***
*** 	void Lego_Interchange(WN* wn_root)
***
***	    Attempt to interchange all outer lego tiles outermost in
***	    the function 'wn_root'.
***
***	void Lego_Peel(WN* wn_root)
***
***	    Carry out the peeling commands indicated in the LEGO_INFO
*** 	    of each loop in the function 'wn_root'.
***
**/

extern BOOL SNL_Legal_Permutation(WN* outer_loop,
				  WN* inner_loop, 
                                  INT permutation[], 
				  INT nloops); 

extern INT Permutation_Last(INT first,
                            INT permutation[],
                            INT nloops);

extern void Mp_Compress_Nested_Loop(WN* wn_loop); 
extern void Lego_Interchange(WN* wn_root); 
extern void Lego_Peel(WN* wn_root); 

