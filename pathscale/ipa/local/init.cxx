/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


// This file contains only Linux-specific code and should be entirely
// #ifdef'd out for Irix.

// Work around the "undefined weak symbol" bug in Linux.
//
// see comments in be/com/weak.cxx.
//
// This file define initialization of pointer variables to symbols defined
// in ipl.so but referenced in be/be.so.

#ifdef __linux__

#include "defs.h"
#include "wn.h"
#include "pu_info.h"
#include "ir_bwrite.h"
#include "ipl_driver.h"
#ifdef KEY
#include "ipl_reorder.h"
#endif

// from ipa/local/ipl_main.cxx
extern void (*Ipl_Extra_Output_p) (Output_File *);
extern void (*Ipl_Init_p) ();
extern void (*Ipl_Fini_p) ();
extern void (*ipl_main_p) (INT, char **);
extern void (*Perform_Procedure_Summary_Phase_p) (WN*, DU_MANAGER*,
						  ALIAS_MANAGER*, void*);
#ifdef KEY
extern void (*Preprocess_struct_access_p) (void);
#endif

struct IPL_INIT
{
    IPL_INIT () {
	Ipl_Extra_Output_p = Ipl_Extra_Output;
	Ipl_Init_p = Ipl_Init;
	Ipl_Fini_p = Ipl_Fini;
	ipl_main_p = ipl_main;
	Perform_Procedure_Summary_Phase_p = Perform_Procedure_Summary_Phase;
#ifdef KEY	// bug 3672
	Preprocess_struct_access_p = Preprocess_struct_access;
#endif
    }
} Ipl_Initializer;

#endif // __linux__
