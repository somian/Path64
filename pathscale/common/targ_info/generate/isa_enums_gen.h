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


// isa_enums_gen.h
/////////////////////////////////////
//
// Interface to create a enum classes.
//
//  void ISA_Create_Enum_Class (const char *ec_name, ...);
//     Create an ENUM CLASS with the <name>, 
//	The subsequent arguments are pairs of strings for the value names
//	and the int bit-value of the enum value.
//     The list is terminated by a NULL string,
//	with either a default value or UNDEFINED.
//	If UNDEFINED, then no default value;
//	otherwise, a default value called "none" has the given value.
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#define UNDEFINED -1

extern void ISA_Enums_Begin (void);

extern void ISA_Create_Enum_Class (const char *ec_name, ...);

extern void ISA_Enums_End (void);

