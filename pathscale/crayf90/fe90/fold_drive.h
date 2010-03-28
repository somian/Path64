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



/* USMID:  "\n@(#)5.0_pl/headers/fold_drive.h	5.2	07/16/99 10:25:58\n" */


# ifdef _TARGET32

int	num_host_wds[Num_Linear_Types] = {
        	/* Err_Res          	*/		0,
        	/* Short_Char_Const 	*/		1,
        	/* Short_Typeless_Const */		1,
        	/* Typeless_1  		*/		1,
        	/* Typeless_2  		*/		1,
        	/* Typeless_4  		*/		1,
        	/* Typeless_8  		*/		2,
        	/* Long_Typeless    	*/		0,
        	/* Integer_1        	*/		1,
        	/* Integer_2        	*/		1,
        	/* Integer_4        	*/		1,
        	/* Integer_8        	*/		2,
        	/* Real_4           	*/		1,
        	/* Real_8           	*/		2,
        	/* Real_16          	*/		4,
        	/* Complex_4        	*/		2,
        	/* Complex_8        	*/		4,
        	/* Complex_16       	*/		8,
        	/* CRI_Ptr_8        	*/		1,
        	/* Logical_1        	*/		1,
        	/* Logical_2        	*/		1,
        	/* Logical_4        	*/		1,
        	/* Logical_8        	*/		2,
        	/* Character_1      	*/		1,
        	/* Character_2      	*/		1,
        	/* Character_4      	*/		1,
        	/* CRI_Ch_Ptr_8     	*/		1,
        	/* Structure_Type   	*/		0,
        	/* CRI_Parcel_Ptr_8 	*/		1,
                };

# else 

int	num_host_wds[Num_Linear_Types] = {
        	/* Err_Res          	*/		0,
        	/* Short_Char_Const 	*/		1,
        	/* Short_Typeless_Const */		1,
        	/* Typeless_1  		*/		1,
        	/* Typeless_2  		*/		1,
        	/* Typeless_4  		*/		1,
        	/* Typeless_8  		*/		1,
        	/* Long_Typeless    	*/		0,
        	/* Integer_1        	*/		1,
        	/* Integer_2        	*/		1,
        	/* Integer_4        	*/		1,
        	/* Integer_8        	*/		1,
        	/* Real_4           	*/		1,
        	/* Real_8           	*/		1,
        	/* Real_16          	*/		2,
# ifdef _WHIRL_HOST64_TARGET64
        	/* Complex_4        	*/		1,
# else
        	/* Complex_4        	*/		2,
# endif /* _WHIRL_HOST64_TARGET64 */
        	/* Complex_8        	*/		2,
        	/* Complex_16       	*/		4,
        	/* CRI_Ptr_8        	*/		1,
        	/* Logical_1        	*/		1,
        	/* Logical_2        	*/		1,
        	/* Logical_4        	*/		1,
        	/* Logical_8        	*/		1,
        	/* Character_1      	*/		1,
        	/* Character_2      	*/		1,
        	/* Character_4      	*/		1,
        	/* CRI_Ch_Ptr_8     	*/		1,
        	/* Structure_Type   	*/		0,
        	/* CRI_Parcel_Ptr_8 	*/		1,
                };
# endif


extern exp_tbl_type     bin_add_tbl[Num_Linear_Types][Num_Linear_Types];
extern long 		input_arith_type[Num_Linear_Types];
