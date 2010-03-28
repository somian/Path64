/* -*- c++ -*-
 *
 * Copyright 2003, 2004, 2005, 2006 PathScale Inc.  All Rights Reserved.


   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

class IPA_BUILTIN {
private:
  PU_Info *_pu_info;	// the function created by IPA
  INTRINSIC _intrinsic;	// id of the intrinsic that the builtin replaces

public:
  IPA_BUILTIN (PU_Info *pu_info, INTRINSIC intrinsic) {
    _pu_info = pu_info;
    _intrinsic = intrinsic;
  }
  PU_Info *Get_PU_Info() {return _pu_info;}
  INTRINSIC Get_Intrinsic() {return _intrinsic;}
};

extern std::vector<IPA_BUILTIN*> IPA_builtins_list;

extern void IPA_Create_Builtins();
extern void IPA_Rename_Builtins(IPA_NODE *);
