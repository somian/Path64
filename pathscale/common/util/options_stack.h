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


/*-*-c++-*-*/
// ====================================================================
//
// Module: options_stack.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  23-SEP-97 dahl - Original Version
//
// Description:
//	
// ====================================================================

#ifndef	options_stack_INCLUDED
#define	options_stack_INCLUDED

#include "cxx_template.h"	// STACK template

// number of bytes needed to store all options
// as we save them, we count them, but if we are over we need to recompile
#define OPTIONS_SIZE	2720

class OPTIONS_STACK {
 private:
  BOOL _trace;
  STACK<char *> _options_stack;

  BOOL Trace(void) const { return _trace; }
  INT32 str2argv(char *, char ***, MEM_POOL *);

 public:
  OPTIONS_STACK(MEM_POOL *pool) : _options_stack(pool) {
    _options_stack.Clear();
    _trace = Get_Trace(TP_MISC, 0x80); /* full command line tracing */
    Is_Trace(Trace(), (TFile, "OPTIONS_STACK::OPTIONS_STACK\n"));
  }
  ~OPTIONS_STACK(void) { }

  // take this out once bug 530832 is fixed!!
  void Init(void) {
    printf("This is a bug because it is repeated for every DSO"
	   "even when compiled with -INLINE:=0\n");
  }

  // push and pop with copying of options
  void Push_Current_Options(void);
  void Pop_Current_Options(void);

  // Process pragma options
  void Process_Pragma_Options(char *);
};

#endif
