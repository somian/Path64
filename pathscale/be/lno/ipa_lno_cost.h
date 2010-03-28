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


#ifndef ipa_lno_cost_INCLUDED
#define ipa_lno_cost_INCLUDED "ipa_lno_cost.h"

#ifndef _IPA_LNO_FILE
#include "ipa_lno_file.h"
#endif 

extern TYPE_ID Cast_Float_Operands(WN** wn_addr_one, WN** wn_addr_two);
extern INT64 Simple_Execution_Cost(WN* wn_call);
extern WN* Execution_Cost(IPA_LNO_READ_FILE* IPA_LNO_File, WN* wn_call, 
  TYPE_ID wtype);

#endif /* ipa_lno_cost_INCLUDED */

