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


// isa_subset_gen.h
/////////////////////////////////////
//
//  Interface for describing which instructions are valid in which ISA subsets.
//
//  void ISA_Subset_Begin( const char* archname )
//      Initialize to generate Instruction Set Architecture subset information
//      for the architecture with the given <archname>.  The information will
//      be written to the files targ_isa_subset.[ch].  
//
//  TYPE ISA_SUBSET
//      An abstract type that represents a subset of the Instruction Set
//      Architecture.  No client visible fields.
//
//  ISA_SUBSET ISA_Subset_Create( ISA_SUBSET isa_subset, const char* name )
//      Used to create a new ISA_SUBTYPE.  <isa_subset> may be either another
//      ISA_SUBSET in which case the newly created set will be a subset of the
//      given <isa_subset>, or it may be NULL, in which case the created set
//      is a root subset.  <name> should be the name for the created
//      ISA_SUBSET and is provided for debugging and documentation purposes.
//
//  void Instruction_Group( ISA_SUBSET subset, ... )
//      Lists the instructions the given <subset>.  Subsequent arguments are
//      TOPs, terminating in TOP_UNDEFINED.  These instructions are also
//      added to all supersets of <subset>.
//
//  void ISA_Subset_End(void)
//      Complete processing.
//
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#ifndef ISA_SUBSET_GEN_INCLUDED
#define ISA_SUBSET_GEN_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#ifndef ISA_SUBSET_GEN_RCS_ID
#define ISA_SUBSET_GEN_RCS_ID
#ifdef _KEEP_RCS_ID
static char *isa_subset_gen_rcs_id = "$Source$ $Revision$";
#endif
#endif

void ISA_Subset_Begin( const char* archname );
typedef struct isa_subset *ISA_SUBSET;
ISA_SUBSET ISA_Subset_Create( ISA_SUBSET subset, const char* name );
void Instruction_Group( ISA_SUBSET isa_subset, ... );
void ISA_Subset_End(void);

#ifdef __cplusplus
}
#endif
#endif
