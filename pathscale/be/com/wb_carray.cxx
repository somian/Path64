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



#ifdef USE_PCH
#include "be_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop
#include <sys/types.h>
#if defined(BUILD_OS_DARWIN)
#include "darwin_elf.h"
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <ctype.h>
#include "wn.h"
#include <stdio.h>
#include "wb_carray.h" 
#include "opt_du.h" 
#include "opt_alias_mgr.h"
#include "wb_util.h" 

WB_CARRAY WB_carray; 

//-----------------------------------------------------------------------
// NAME: WB_CARRAY::Enter_This_Node
// FUNCTION: Enter the node 'wn' into the saved node array '_carray'.
//   Update the value of '_next_index', the index of the next available 
//   entry in '_carray'.  
// NOTE: The 'wn' is not entered into the 'carray' if it is already full.
//-----------------------------------------------------------------------

void WB_CARRAY::Enter_This_Node(WN* wn) 
{
  if (_next_index < WB_MAX_SAVED_NODES) 
    _carray[_next_index] = wn;
  _next_index++;
}

//-----------------------------------------------------------------------
// NAME: WB_CARRAY::Enter_This_Node_Unique
// FUNCTION: Same as 'enter_this_node', except the 'wn' is not entered
//   if it is already there.
//-----------------------------------------------------------------------

INT WB_CARRAY::Enter_This_Node_Unique(WN* wn)
{
  INT i;
  for (i = 0; i < _next_index; i++)
    if (wn == _carray[i])
      break;
  if (i < _next_index)
    return i;
  if (_next_index < WB_MAX_SAVED_NODES)
    _carray[_next_index] = wn;
  _next_index++;
  return _next_index - 1; 
}

