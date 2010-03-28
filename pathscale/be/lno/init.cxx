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


// This file contains only Linux-specific code and should be entirely
// #ifdef'd out for Irix.

// Work around the "undefined weak symbol" bug in Linux.
//
// see comments in be/com/weak.cxx.
//
// This file define initialization of pointer variables to symbols defined
// in lno.so but referenced in be/be.so.

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

#include "defs.h"
#include "wn.h"
#include "pu_info.h"
#include "lnodriver.h"

extern void (*lno_main_p) (INT, char**, INT, char**);
extern void (*Lno_Init_p) ();
extern void (*Lno_Fini_p) ();
extern WN* (*Perform_Loop_Nest_Optimization_p) (PU_Info*, WN*, WN*, BOOL);

struct LNO_INIT
{
    LNO_INIT () {
	lno_main_p = lno_main;
	Lno_Init_p = Lno_Init;
	Lno_Fini_p = Lno_Fini;
	Perform_Loop_Nest_Optimization_p = Perform_Loop_Nest_Optimization;
    }
} Lno_Initializer;

#endif // __linux__
