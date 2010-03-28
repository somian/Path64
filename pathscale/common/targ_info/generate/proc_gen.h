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


// proc_gen.h
/////////////////////////////////////
//
// Interface to create a new PROC (actually just an enum of all the processors).
//
//  void PROC_Create (const char *proc_name, ...);
//     Create an PROC with the <name>, The subsequent arguments are strings
//     that are the names of the instructions. The list is terminated by a 
//     NULL string.
//
//  The output of PROC_Create is to generate targ_proc.[ch], which define:
//
//     PROCESSOR stands for Target Processor; prefix is PROCESSOR.
//
//     enum PROCESSOR
//         Containins all the target processors.  Their names have the form 
//         PROCESSOR_<name>.
//
//     const PROCESSOR PROCESSOR_UNDEFINED
//         Useful value guaranteed not to be a valid PROCESSOR.
//
//     const int PROCESSOR_count
//         Gives the number of processors.
//
//     char* PROCESSOR_Name(PROCESSOR proc)
//         Returns the name of the given PROCESSOR.
//    
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$

extern void PROC_Create (const char *proc_name, ...);
