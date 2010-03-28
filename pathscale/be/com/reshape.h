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


#ifndef reshape_INCLUDED
#define reshape_INCLUDED "ipa_reshape.h"

class RESHAPE
{

private:
  PROJECTED_REGION *_callee_proj_region;
  PROJECTED_REGION *_callsite_region;
  PROJECTED_REGION *_caller_shape_proj_region;
  PROJECTED_REGION *_callee_shape_proj_region;
  PROJECTED_REGION *_callee_proj_reshaped_region;
  MEM_POOL* _m;
 
public:

  BOOL Constant_Type_Reshape();

  PROJECTED_REGION* Reshape_Constant_Shape(BOOL trace = FALSE);

  RESHAPE(PROJECTED_REGION* caller_shape_proj_region,
    PROJECTED_REGION* callee_shape_proj_region,
    PROJECTED_REGION* callee_proj_region,
    PROJECTED_REGION* callsite_region,
    MEM_POOL *m,
    BOOL trace = FALSE);

  PROJECTED_REGION* Reshape_Callee_To_Caller(BOOL trace = FALSE);
                                                            
  BOOL Reshapeable_Passed_Section(BOOL trace = FALSE);
                                                            
  void Reshape_Passed_Section(PROJECTED_REGION* pr, BOOL trace = FALSE);

  void Set_callee_proj_reshaped_region(PROJECTED_REGION* pr) 
    {_callee_proj_reshaped_region  = pr;};

  ~RESHAPE() {};
};

#endif /* reshape_INCLUDED */

