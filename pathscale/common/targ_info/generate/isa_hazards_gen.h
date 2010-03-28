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


// isa_hazards_gen.h
/////////////////////////////////////
//
//  Interface for describing instructions that have ISA hazards, and what
//  the hazard is.
//
//  void ISA_Hazards_Begin( const char* archname )
//      Initialize to generate Instruction Set Architecture hazard information
//      for the architecture with the given <archname>.  The information will
//      be written to the files <archname>_hazards.[ch].  
//
//  TYPE ISA_HAZARD
//      An abstract type that represents a type of ISA hazard.
//      No client visible fields.
//
//  ISA_HAZARD Hazard_Create( const char *name )
//      Used to create a new ISA_HAZARD. <name> is used to construct
//      an enumeration constant for this hazard in the generated output
//	of the form: ISA_HAZARD_<name>
//
//  void Hazard_Group( TOP topcode, ... )
//      Lists a group of instructions (by TOP), with a common hazard 
//	specification, terminating in TOP_UNDEFINED. Subsequent
//	statements specify the details of the hazard. The specification
//	is terminated by another Hazard_Group, or ISA_Hazards_End.
//
//  void Hazard_Type( ISA_HAZARD isa_hazard )
//	Indicates the type of the hazard.
//
//  void Hazard_Data( int data )
//	One word of hazard-specific data, e.g. an operand number.
//
//  void Hazard_Post_Ops( int ops )
//	The number of OPs which must follow an OP in the hazard group
//
//  void Hazard_Pre_Ops( int ops )
//	The number of OPs which must precede an OP in the hazard group
//
//  void Hazard_ISA( ISA_SUBSET isa_subset )
//	Not all ISAs have the same hazards. Hazard_ISA specifies that
//	'isa_subset' has the current hazard. Hazard_ISA may be called
//	multiple times per hazard group.
//
//  void ISA_Hazards_End(void)
//      Complete processing.
//
//
/////////////////////////////////////


//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_hazards_gen.h $


#ifndef ISA_HAZARDS_GEN_INCLUDED
#define ISA_HAZARDS_GEN_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#ifndef ISA_HAZARDS_GEN_RCS_ID
#define ISA_HAZARDS_GEN_RCS_ID
#ifdef _KEEP_RCS_ID
static const char isa_hazards_gen_rcs_id[] = "$Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.isa_hazards_gen.h $ $Revision: 1.5 $";
#endif
#endif

void ISA_Hazards_Begin( const char* archname );
typedef struct isa_hazard *ISA_HAZARD;
ISA_HAZARD Hazard_Create( const char *name );
void Hazard_Group( TOP topcode, ... );
void Hazard_Type( ISA_HAZARD isa_hazard );
void Hazard_Data( int data );
void Hazard_Post_Ops( int ops );
void Hazard_Pre_Ops( int ops );
void Hazard_ISA( ISA_SUBSET isa_subset );
void ISA_Hazards_End(void);

#ifdef __cplusplus
}
#endif
#endif
