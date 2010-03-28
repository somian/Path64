/*
 *  Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004 PathScale, Inc.  All Rights Reserved.
 */

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


//  
//  Generate ABI information
///////////////////////////////////////


//  $Revision: 1.7 $
//  $Date: 05/03/02 15:27:50-08:00 $
//  $Author: bos@eng-24.internal.keyresearch.com $
//  $Source: common/targ_info/abi/mips/SCCS/s.abi_properties.cxx $

#include <stddef.h>
#include "abi_properties_gen.h"
#include "targ_isa_registers.h"

static ABI_PROPERTY
  allocatable,
  callee,
  caller,
  func_arg,
  func_val,
  stack_ptr,
  frame_ptr,
  static_link,
  global_ptr,
  entry_ptr,
  zero,
  ret_addr;

static void n32_abi(void)
{
  static const char *integer_names[32] = {
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   "$t9",  NULL,   NULL,   "$gp",  "$sp",  "$fp",  NULL,
  };

  Reg_Names(ISA_REGISTER_CLASS_integer, 0, 31, integer_names);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_integer,
		      1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  20,  21,  22,  23,
		24,  25,                      30,  31,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_integer,
                16,  17,  18,  19,  20,  21,  22,  23,
					      30,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_integer,
		      1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                
		24,  25,  			   31,
	       -1);
  Reg_Property(func_arg, ISA_REGISTER_CLASS_integer,
			             4,   5,   6,   7,  
		 8,   9,  10,  11, 
	       -1);
  Reg_Property(func_val, ISA_REGISTER_CLASS_integer,
			   2,   3, 
	       -1);
  Reg_Property(static_link, ISA_REGISTER_CLASS_integer,
			   2,   
	       -1);
  Reg_Property(global_ptr, ISA_REGISTER_CLASS_integer, 
		                    28,
	       -1);
  Reg_Property(entry_ptr, ISA_REGISTER_CLASS_integer, 
		                    25,
	       -1);
  Reg_Property(stack_ptr, ISA_REGISTER_CLASS_integer, 
		                        29,
	       -1);
  Reg_Property(frame_ptr, ISA_REGISTER_CLASS_integer, 
		                            30,
	       -1);
  Reg_Property(ret_addr, ISA_REGISTER_CLASS_integer, 
		                                31,
	       -1);
  Reg_Property(zero, ISA_REGISTER_CLASS_integer, 
		0,
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_float,
		 0,   1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  20,  21,  22,  23,
		24,  25,  26,  27,  28,  29,  30,  31,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_float,
                                    20,       22, 
		24,       26,       28,       30,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_float,
		 0,   1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,       21,       23,
		     25,       27,       29,       31,
	       -1);
  Reg_Property(func_arg, ISA_REGISTER_CLASS_float,
		                    12,  13,  14,  15,
                16,  17,  18,  19, 
	       -1);
  Reg_Property(func_val, ISA_REGISTER_CLASS_float,
		 0,        2, 
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_fcc,
		 0,   1,   2,   3,   4,   5,   6,   7,  
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_fcc,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_fcc,
		 0,   1,   2,   3,   4,   5,   6,   7,  
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_hilo,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_hilo,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_hilo,
	       -1);
}

static void n64_abi(void)
{
  static const char *integer_names[32] = {
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   "$t9",  NULL,   NULL,   "$gp",  "$sp",  "$fp",  NULL,
  };

  Reg_Names(ISA_REGISTER_CLASS_integer, 0, 31, integer_names);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_integer,
		      1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  20,  21,  22,  23,
		24,  25,                      30,  31,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_integer,
                16,  17,  18,  19,  20,  21,  22,  23,
					      30,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_integer,
		      1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                
		24,  25,  			   31,
	       -1);
  Reg_Property(func_arg, ISA_REGISTER_CLASS_integer,
			             4,   5,   6,   7,  
		 8,   9,  10,  11, 
	       -1);
  Reg_Property(func_val, ISA_REGISTER_CLASS_integer,
			   2,   3, 
	       -1);
  Reg_Property(static_link, ISA_REGISTER_CLASS_integer,
			   2,   
	       -1);
  Reg_Property(global_ptr, ISA_REGISTER_CLASS_integer, 
		                    28,
	       -1);
  Reg_Property(entry_ptr, ISA_REGISTER_CLASS_integer, 
		                    25,
	       -1);
  Reg_Property(stack_ptr, ISA_REGISTER_CLASS_integer, 
		                        29,
	       -1);
  Reg_Property(frame_ptr, ISA_REGISTER_CLASS_integer, 
		                            30,
	       -1);
  Reg_Property(ret_addr, ISA_REGISTER_CLASS_integer, 
		                                31,
	       -1);
  Reg_Property(zero, ISA_REGISTER_CLASS_integer, 
		0,
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_float,
		 0,   1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  20,  21,  22,  23,
		24,  25,  26,  27,  28,  29,  30,  31,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_float,
		24,  25,  26,  27,  28,  29,  30,  31,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_float,
		 0,   1,   2,   3,   4,   5,   6,   7,  
		 8,   9,  10,  11,  12,  13,  14,  15,
                16,  17,  18,  19,  20,  21,  22,  23,
	       -1);
  Reg_Property(func_arg, ISA_REGISTER_CLASS_float,
		                    12,  13,  14,  15,
                16,  17,  18,  19, 
	       -1);
  Reg_Property(func_val, ISA_REGISTER_CLASS_float,
		 0,        2, 
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_fcc,
		 0,   1,   2,   3,   4,   5,   6,   7,  
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_fcc,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_fcc,
		 0,   1,   2,   3,   4,   5,   6,   7,  
	       -1);

  Reg_Property(allocatable, ISA_REGISTER_CLASS_hilo,
	       -1);
  Reg_Property(callee, ISA_REGISTER_CLASS_hilo,
	       -1);
  Reg_Property(caller, ISA_REGISTER_CLASS_hilo,
	       -1);
}

main()
{
  ABI_Properties_Begin("MIPS");

  allocatable = Create_Reg_Property("allocatable");
  callee = Create_Reg_Property("callee");
  caller = Create_Reg_Property("caller");
  func_arg = Create_Reg_Property("func_arg");
  func_val = Create_Reg_Property("func_val");
  stack_ptr = Create_Reg_Property("stack_ptr");
  frame_ptr = Create_Reg_Property("frame_ptr");
  static_link = Create_Reg_Property("static_link");
  global_ptr = Create_Reg_Property("global_ptr");
  entry_ptr = Create_Reg_Property("entry_ptr");
  zero = Create_Reg_Property("zero");
  ret_addr = Create_Reg_Property("ret_addr");
  
  Begin_ABI("n32");
  n32_abi();
  
  Begin_ABI("n64");
  n64_abi();

  ABI_Properties_End();
}
