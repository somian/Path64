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


#ifndef const_INCLUDED
#define const_INCLUDED


#ifndef targ_const_INCLUDED
#include "targ_const.h"
#endif /* targ_const_INCLUDED */

class WN;

extern ST *
New_Const_Sym (TCON_IDX tcon, TY_IDX ty);


extern ST *
Gen_String_Sym (TCON *val, TY_IDX ty, BOOL unique);


extern TCON Const_Val (WN *n);

extern WN *
Make_Const (TCON c);

static const INT32 MAX_SYMBOLIC_CONST_NAME_LEN = 1024;

#ifndef MONGOOSE_BE

extern WN *
Make_Zerocon ( TYPE_ID mtype );

extern WN *
Make_Comparison_Result_Const ( INT16 val );

extern WN *
Make_Integer_Const ( INT16 mtype, TARG_INT val );

#endif /* MONGOOSE_BE */

extern WN *Make_Reduction_Identity ( INT32 opr, TYPE_ID mtype );

#endif /* const_INCLUDED */
