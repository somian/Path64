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


// isa_pack_gen.h
/////////////////////////////////////
//
//  Interface for packing instructions into an instruction word for all
//  instructions in the ISA.
//
//  void ISA_Pack_Begin( const char* archname, int inst_bit_size )
//      Initialize to generate instruction packing information for the 
//      architecture with the given <archname>.  The information will be 
//	written to the files targ_isa_pack.[ch].  <inst_bit_size>
//	indicates how many bits in an instruction (operation) word.
//
//  TYPE ISA_PACK_TYPE
//      An abstract type that describes different types of packing formats.
//
//  ISA_PACK_TYPE ISA_Pack_Type_Create( const char* name )
//      Used to create a new ISA_PACK_TYPE.  <name> is the pack_type name.
//	The rules to pack individual operands/results are added using the
//      following two routines.
//
//  TYPE OPND_ADJ_TYPE
//	An abstract type that describes different types of operand adjustments.
//
//  OPND_ADJ_TYPE Create_Operand_Adjustment(const char *name,
//					    const char *adj)
//	Used to create a new operand adjustment. <name> is a descriptive
//	name to be used in comments. <adj> is a string whose contents
//	is a C expression that applies an adjustment to the value
//	of an operand. The macro O_VAL is used to refer to the operand
//	value. The expression should produce a value that is to
//	replace the original value.
//
//  void Instruction_Pack_Group(ISA_PACK_TYPE pack_type, ...)
//      Group instructions which have the same packing format but initialize
//	<opcode_mask> separately for each TOP in this group. The variable
//	part of the parameter list is the pair:
//
//		TOP top, <unsigned-type> opcode_mask
//
//	<unsigned-type> is an unsigned type large enough to hold the
//	instruction word (must correspond with the inst_size parameter
//	of ISA_Pack_Begin). The parameter list is ended by specifing 
//	TOP_UNDEFINED for <top> (<opcode_mask> may be omitted on the 
//	end of list marker).
//
//  void Operand (int operand_index,
//		  int opnd_position, 
//		  int inst_position,
//		  int width)
//	The <operand_index>'th operand of the current packing type is 
//	<width> bits wide. The bits are extracted from the operand at
//	bit <opnd_position> and deposited into the instruction word
//	at bit <inst_position>.
//
//	<opnd_position> will only be non-zero in cases where an operand
//	is split amoung multiple bit fields of an instruction word.
//	In such cases you would include multiple Operand specifications with
//	the same <operand_index>, one for each field.
//
//  void Adjust_Operand(int operand_index, 
//			OPND_ADJ_TYPE pack_adj,
//			OPND_ADJ_TYPE unpack_adj)
//	The <operand_index>'th operand of the current packing type
//	requires an adjustment when going between assembler and binary
//	forms. <pack_adj> specifies the adjustment when packing a binary
//	instruction, and <unpack_adj> specifies the adjustment when
//	unpacking a binary instruction.
//
//  void Result (int result_index, int bit_position, int width)
//	The <result_index>'th result of the current packing type is
//	at <bit_position> in the instruction word and is <width> bits long.
//
//  void Next_Word (void)
//	In multiple word instructions, advance to the next word. Subsequent
//	Operand and Result calls operate on this word.
//
//  void ISA_Pack_End(void)
//      Complete processing of operands/results.
//
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifndef isa_pack_gen_INCLUDED
#define isa_pack_gen_INCLUDED

#ifdef _KEEP_RCS_ID
static const char isa_operands_gen_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct isa_pack_type *ISA_PACK_TYPE;

typedef struct opnd_adj_type *OPND_ADJ_TYPE;

extern void ISA_Pack_Begin ( const char* archname, int inst_bit_size );
extern ISA_PACK_TYPE ISA_Pack_Type_Create ( const char* name );
extern OPND_ADJ_TYPE Create_Operand_Adjustment(const char *name,
					       const char *adj);
extern void Instruction_Pack_Group (ISA_PACK_TYPE pack_type, ...);
extern void Operand (int operand_index,
		     int opnd_position, 
		     int inst_position, 
		     int width);
extern void Adjust_Operand(int operand_index, 
			   OPND_ADJ_TYPE pack_adj,
			   OPND_ADJ_TYPE unpack_adj);
extern void Result (int result_index, int bit_position, int width);
extern void Next_Word (void);
extern void ISA_Pack_End(void);

#ifdef __cplusplus
}
#endif
#endif /* isa_pack_gen_INCLUDED */
