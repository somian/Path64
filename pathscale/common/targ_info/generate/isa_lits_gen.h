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


// isa_lits_gen.h
/////////////////////////////////////
//
// Interface to create literal classes.
//
//  void ISA_Lits_Begin(void)
//      Initialize to generate literal class information for an architecture.
//      The information will be written to the files targ_isa_lits.[ch].  
//
//  TYPE LIT_RANGE
//      An abstract type that represents a literal range.
//      No client visible fields.
//
//  const LIT_RANGE LIT_RANGE_END
//	A special LIT_RANGE value that is used to mark the end of a
//	list of LIT_RANGE parameters.
//
//  LIT_RANGE ISA_Create_Lit_Range(const char *name, 
//				   long long min, long long max)
//	Create a literal range named <name> (used for debugging/informational
//	purposes only) with minimum value <min> and maximum value <max>.
//
//  LIT_RANGE SignedBitRange(unsigned int bit_size)
//  LIT_RANGE UnsignedBitRange(unsigned int bit_size)
//	Create a signed/unsigned literal range for a bit-field of size
//	<bit_size>.
//
//  TYPE (enum) LIT_CLASS_TYPE
//	An enumeration of the types of literals: eg, UNSIGNED or SIGNED.
//
//  void ISA_Create_Lit_Class (const char *name, LIT_CLASS_TYPE type, ...)
//	Create a LIT CLASS with the <name> and type <type>. The variable
//	argument list contains a sequence of LIT_RANGE parameters to
//	identify one or more disjoint ranges that are legal for the
//	literal class. The list is ended with a parameter of LIT_RANGE_END.
//
//  void ISA_Lits_End(void)
//      Complete processing of literals.
//
/////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifndef isa_lits_gen_INCLUDED
#define isa_lits_gen_INCLUDED

#ifdef _KEEP_RCS_ID
static const char isa_lits_gen_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UNSIGNED,
  SIGNED
} LIT_CLASS_TYPE;

typedef struct lit_range *LIT_RANGE;

#define LIT_RANGE_END ((LIT_RANGE)0)

extern void ISA_Lits_Begin (void);

extern LIT_RANGE ISA_Create_Lit_Range(const char *name, 
				      long long min, 
				      long long max);

extern LIT_RANGE SignedBitRange(unsigned int bit_size);
extern LIT_RANGE UnsignedBitRange(unsigned int bit_size);

extern void ISA_Create_Lit_Class (const char *name, LIT_CLASS_TYPE type, ...);

extern void ISA_Lits_End (void);

#ifdef __cplusplus
}
#endif
#endif /* isa_lits_gen_INCLUDED */
