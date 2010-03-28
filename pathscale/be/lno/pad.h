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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef pad_INCLUDED
#define pad_INCLUDED
#define USE_STANDARD_TYPES
#ifndef defs_INCLUDED
#include "defs.h"
#endif

#ifndef cxx_base_INCLUDED
#include "cxx_base.h"
#endif

#ifndef cxx_template_INCLUDED
#include "cxx_template.h"
#endif


//======================================================
// 
// And array describing the bounds information
//
//======================================================
class BOUNDS
{
private:
 static const UINT8 IS_CONSTANT_BOUNDS = 0x1;
 INT64 _upper, _lower, _stride;
 UINT8 _flags;

public:
 void Init() { _upper = _lower = _stride = 0;};
 void Init(INT64 upper, INT64 lower, INT64 stride) {
    _upper = upper; _lower = lower; _stride = stride;
  };

 void Set_Constant() { _flags = _flags | IS_CONSTANT_BOUNDS; };
 BOOL Is_Constant() { return _flags & IS_CONSTANT_BOUNDS;};

 INT64 Get_Upper() { return _upper;};
 INT64 Get_Lower() { return _lower;};
 INT64 Get_Stride() { return _stride;};
 void Set_Upper(INT64 upper) { _upper = upper;};
 void Set_Lower(INT64 lower) { _lower = lower;};
 void Set_Stride(INT64 stride) { _stride = stride;};

};

typedef DYN_ARRAY<BOUNDS> BOUNDS_ARRAY;


#endif
