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


// abi_properties_gen.h
/////////////////////////////////////
//
//  Interface for specifying properties (attributes) for various ABIs
//  supported by an architecture.
//
//  void ABI_Properties_Begin(const char *name)
//	Initialize to generate ABI properties information for the architecture 
//	with the given <name>.  The information will be written to the 
//	files targ_abi_properties.[ch].  
//
//  TYPE ABI_PROPERTY
//	An abstract type that represents a property of an ABI.
//	No client visible fields.
//
//  ABI_PROPERTY Create_Reg_Property(const char *name)
//	Used to create a new ABI_PROPERTY.  <name> is the property name.
//	It will be used to define a ABI_PROPERTY_<name> constant.
//
//  Begin_ABI(const char *name)
//	Begin the definition of the ABI named <name>. An ABI definition
//	is ended by another Begin_ABI() call or a call to ABI_Properties_End().
//
//  void Reg_Property(ABI_PROPERTY prop, ISA_REGISTER_CLASS rc, ...)
//	Give a number of registers in class <rc>, the ABI property <prop>,
//	i.e. it identifies the role of particular registers in the ABI.
//	The variable argument specifies the register numbers, terminated
//	by -1.
//
//  void Reg_Names(ISA_REGISTER_CLASS rc, 
//		   INT minreg, 
//		   INT maxreg, 
//		   const char **names)
//	Give ABI names to the registers in class <rc>. These names typically
//	identify the role of the register in the ABI. If a register is
//	not explicitly given an ABI name, the hardware name is used.
//	The register names are specified in an array pointed to by <names>.
//	<minreg> gives the register number of the first name in the
//	<names> array; <maxreg> gives the last.
//
//  void ABI_Properties_End(void)
//      Complete processing of ABI properties.
//
/////////////////////////////////////
//
//  $Revision: 1.5 $
//  $Date: 04/12/21 14:57:26-08:00 $
//  $Author: bos@eng-25.internal.keyresearch.com $
//  $Source: /home/bos/bk/kpro64-pending/common/targ_info/generate/SCCS/s.abi_properties_gen.h $

#include "targ_isa_registers.h"

typedef struct abi_property *ABI_PROPERTY;

extern void ABI_Properties_Begin(const char *name);
extern ABI_PROPERTY Create_Reg_Property(const char *name);
extern void Begin_ABI(const char *name);
extern void Reg_Property(ABI_PROPERTY prop, ISA_REGISTER_CLASS rc, ...);
extern void Reg_Names(ISA_REGISTER_CLASS rc, 
		      INT minreg, 
		      INT maxreg, 
		      const char **names);
extern void ABI_Properties_End(void);
