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


// proc_properties_gen.h
/////////////////////////////////////
//
//  Interface for specifying properties (attributes) for various
//  instructions in the PROC.
//
//  void PROC_Properties_Begin( const char* archname )
//      Initialize to generate properties information for the architecture 
//      with the given <archname>.  The information will be written to the 
//      files targ_proc_properties.[ch].  
//
//  TYPE PROC_PROPERTY
//      An abstract type that represents a property of a processor.
//      No client visible fields.
//
//  PROC_PROPERTY PROC_Property_Create( const char* name )
//      Used to create a new PROC_PROPERTY.  <name> is the property name.
//      It will be used to define a PROCESSOR_<name> access function.
//
//  void Processor_Group( PROC_PROPERTY property, ... )
//      Lists the processors with the given <property>. Subsequent arguments 
//      are PROCs, terminating in PROCESSOR_UNDEFINED.  
//
//  void PROC_Properties_End(void)
//      Complete processing of properties.
//
/////////////////////////////////////
//
//  $Revision$
//  $Date$
//  $Author$
//  $Source$

typedef struct proc_property *PROC_PROPERTY;

extern void PROC_Properties_Begin( const char* archname );
extern PROC_PROPERTY PROC_Property_Create( const char* name );
extern void Processor_Group( PROC_PROPERTY property, ... );
extern void PROC_Properties_End(void);
