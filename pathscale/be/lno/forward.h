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
*** Description: 
***
***   This file contains the description of the forward and backward substi-
***   tution optimizations provided in the LNO phase of the optimizer. They  
***   are different and more general than that provided in the preopt phase, 
***   since it uses the dependence graph computed at the beginning of LNO.
***
***   Currently we will forward substitute any scalar references which 
***   are defined within a loop and have at least one use which is nested
***   in a deeper loop.  For example, on the following code: 
***
***     subroutine fs(a, b, c, n) 
***     integer a(n,n), b(n,n,n), c(n) 
***     integer i, j, n, x, y, z 
***     do i = 2, n-1 
***       z = c(i-1)  
***       c(i) = 15 
***       do j = 1, n 
***         x = a(i,j)
***         y = a(i+1,j)
***         a(i-1,j) = 10  
***         do k = 1, n 
***           b(i,j,k) = x + y + z  
***         end do
***       end do  
***     end do 
***     end 
***
***   We will effectly translate it into: 
***
***     subroutine fs(a, b, c, n) 
***     integer a(n,n), b(n,n,n), c(n) 
***     integer i, j, n, x, y, z 
***     do i = 2, n-1 
***       c(i) = 15 
***       do j = 1, n 
***         a(i-1,j) = 10  
***         do k = 1, n 
***           b(i,j,k) = a(i,j) + a(i+1,j) + c(i-1)  
***         end do
***       end do  
***     end do 
***     end 
***
***   We assume that the "minvariant" transformation performed after 
***   this optimization will remove any invariant references that it has 
***   placed within the loop.  The basic idea is that by performing this 
***   forward substitution, we make it easier to create perfect nests and 
***   to choose an appropriate loop order for SNLs.  We then allow min-      
***   variant phase to remove the new invariant references after the SNLs
***   have been transformed. 
***  
***   The constant FS_LOAD_LIMIT indicates how many loads are allowed in
***   in the expression being substituted. 
***
***   In backward substitution, we look for statements of the form 
***   "array = scalar", and backward substitute the scalar references 
***   if some of them occur in a deeper loop and the array reference 
***   is not reassigned over the life of the scalar's equivalence class. 
***   This lets us transform the following: 
***
***     subroutine fs(a, b, c, n) 
***     integer i, j, k, n 
***     real s, a(n,n), b(n,n), c(n,n)
***     do i = 1, n 
***       do j = 1, n
***        s = c(i,j)  
***        do k = 1, n
***           s = s + a(i,k) * b(k,j)  
***        end do
***        c(i,j) = s  
***      end do 
***     end do 
***     end 
***
***   into: 
***
***     subroutine fs(a, b, c, n) 
***     integer i, j, k, n 
***     real s, a(n,n), b(n,n), c(n,n)
***     do i = 1, n 
***       do j = 1, n
***        do k = 1, n
***           c(i,j) = c(i,j) + a(i,k) * b(k,j)  
***        end do
***      end do 
***     end do 
***     end 
***
***  For innermost loops, we will forward substitute arbitrary scalar 
***  expressions if there is only one use of the expression. (i.e. 
***  FS_LOAD_LIMIT is not used to test whether the substituted expression 
***  is too compilcated.)  For example: 
***
***	subroutine fs(a, b, c, d, n) 
***	integer i, n 
*** 	real a(n), b(n), c(n), d(n)
***	do i = 1, n 
***       s = a(i) + b(i) 
***	  c(i) = s + d(i) 
***	end do 
***	end  
***
***   becomes: 
***
***	subroutine fs(a, b, c, d, n) 
***	integer i, n 
*** 	real a(n), b(n), c(n), d(n)
***	do i = 1, n 
***	  c(i) = a(i) + b(i) + d(i) 
***	end do 
***
***  Exported types and functions:
*** 
***	void Array_Substitution(WN* func_nd)
***
***	    Forward substitute array references for the function 'func_nd'.
***
***	void Forward_Substitute_Ldids(WN* wn_exp, DU_MANAGER* du)
***
***	    Forward substitutes each LDID in 'wn_exp' which has a single
***	    definition which is simple enough.  Rebuild the access vectors    
***	    of the immediately enclosing loop or if when an ldid is forward 
***	    substituted.
***  
**/

#ifndef forward_INCLUDED
#define forward_INCLUDED "forward.h"

extern const INT FS_LOAD_LIMIT; 
extern void Array_Substitution(WN* func_nd); 
extern void Forward_Substitute_Ldids(WN* wn_exp, DU_MANAGER* du); 
extern WN* Forward_Substitutable(WN* wn_ldid, DU_MANAGER* du);

#endif /* forward_INCLUDED */ 
