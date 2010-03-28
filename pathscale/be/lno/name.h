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



/** $Revision$
*** $Date$
*** $Author$
*** $Source$
**/

#ifndef NAME_RCS_ID
#define NAME_RCS_ID
#ifdef _KEEP_RCS_ID
static char *name_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */
#endif

#ifndef NAME_DECLARE
#define NAME_DECLARE

#include "defs.h"
#include "wn.h"
#include "cxx_memory.h"
#include "cxx_template.h"
#include "access_vector.h"
#include "opt_alias_interface.h"

// class NAME2BIT is used internally in inner_fission phase.
// It contains the mapping from a symbol to a bit-positionin the bit
// vector used to record the names of the variables in a statement or SCC.
// It also defines the comparison operators (<,==,>) so that it can be
// used as nodes in the binary tree (be/com/btree.h).

class NAME2BIT {
  SYMBOL 		_symbol;
  ACCESS_ARRAY*	_access_array;
  UINT   		_bit_position;
public:
  NAME2BIT() { _access_array = NULL; }
  NAME2BIT(const SYMBOL symbol) { _symbol=symbol; _access_array = NULL; }
  NAME2BIT(const WN* wn) { _symbol.Init(wn); _access_array = NULL; }
  ~NAME2BIT() {}
  BOOL operator <(const NAME2BIT &name2bit) const;
  BOOL operator ==(const NAME2BIT &name2bit) const;
  BOOL operator >(const NAME2BIT &name2bit) const {
    return (!(*this < name2bit) && !(*this == name2bit));
  }
  void Set_Bit_Position(const UINT pos) { _bit_position=pos; }
  void Set_Symbol(const SYMBOL symbol) { _symbol=symbol; }
  void Set_Symbol(const WN* wn) { _symbol.Init(wn); }
  void Set_Access_Array(ACCESS_ARRAY *ar) { _access_array = ar; }
  UINT64 Get_Bit_Position() const { return _bit_position; }
  SYMBOL Get_Symbol() const { return _symbol; }
  ACCESS_ARRAY* Get_Access_Array() const { return _access_array; }
};


#endif 

