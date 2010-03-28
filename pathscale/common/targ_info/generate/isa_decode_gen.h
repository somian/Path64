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


// isa_decode_gen.h
/////////////////////////////////////
//
//  Interface for decoding instructions.
//
//  The decoding interface is specification is in the form of a state
//  machine. Each intermediate (non-final) state examines a bit field
//  of the instruction word and uses that to determine the next state.
//  When a final state is reached, that state will specify the topcode
//  for the instruction.
//
//  void ISA_Decode_Begin(const char* archname)
//      Initialize to generate instruction decoding information for the 
//      architecture with the given <archname>.  The information will be 
//	written to the files targ_isa_decode.[ch].  
//
//  void ISA_Decode_End(void)
//      Complete processing of decoding information.
//
//  TYPE STATE
//	This type holds the information about a state. It's content
//	are hidden from the client.
//
//  STATE Create_Unit_State(const char *tag, int pos, int width)
//  STATE Create_Inst_State(const char *tag, int idx, int pos, int width)
//	Create a intermediate (non-final) state. <tag> is a short string
//	to uniquely identify the state. The string is used as part of
//	an identifier, so it should only contain characters legal in an
//	identifier. Create_Unit_State identifies that this state
//	transitions based on bits in the execution unit. Create_Inst_State
//	identifies that this state transitions based on bits in the
//	instruction. <pos> and <width> specify a bit field in the 
//	instruction word or the execution unit that will determine the 
//	next state. <idx> determines what instruction word contains
//	the specified bit field.
//
//  STATE Final(TOP topcode)
//	Creates a final state. <topcode> specifies the resulting
//	topcode for the decode operation.
//
//  const int END_TRANSITIONS
//	Marks the end of a transition list. See the description for
//	Transitions() for details.
//
//  void Transitions(STATE state, ...)
//	Specifies the transitions for <state>. Each transition is
//	specified by the pair: int num, STATE newstate. <num> specifies
//	the transition number (corresponds to the bitfield specified
//	in Create_State). <newstate> is the state to move to when
//	the value of the bitfield matches the transition number.
//	The list of transitions is terminated by passing the constant
//	END_TRANSITIONS for <num>.
//
//  void Initial_State(STATE state)
//	This sets the initial state for the instruction decoder.
//
/////////////////////////////////////

//  $Revision$
//  $Date$
//  $Author$
//  $Source$

#ifndef isa_decode_gen_INCLUDED
#define isa_decode_gen_INCLUDED

#ifdef _KEEP_RCS_ID
static const char isa_decode_gen_rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif

extern void ISA_Decode_Begin(const char* archname);
extern void ISA_Decode_End(void);

typedef struct state *STATE;
enum { END_TRANSITIONS = -1 };
extern STATE Create_Unit_State(const char *tag, int pos, int width);
extern STATE Create_Inst_State(const char *tag, int idx, int pos, int width);
extern void Transitions(STATE state, ...);
extern void Initial_State(STATE state);
extern STATE Final(TOP topcode);

#ifdef __cplusplus
}
#endif
#endif /* isa_decode_gen_INCLUDED */
