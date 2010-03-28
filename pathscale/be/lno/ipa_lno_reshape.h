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


#ifndef ipa_lno_reshape_INCLUDED
#define ipa_lno_reshape_INCLUDED "ipa_lno_reshape.h"

extern PROJECTED_REGION* Map_Formal(IPA_LNO_READ_FILE* IPA_LNO_File,
                                    WN* wn_call,
                                    INT idx_formal);

extern PROJECTED_REGION* Projected_Region(IPA_LNO_READ_FILE* IPA_LNO_File,
                                          INT idx_formal,
					  WN* wn_call);

extern PROJECTED_REGION* Map_Projected_Region(IPA_LNO_READ_FILE* IPA_LNO_File,
                                              PROJECTED_REGION* pr_memory,
                                              WN* wn_call);

extern BOOL Shape_Mismatch_At_Formal(IPA_LNO_READ_FILE* IPA_LNO_File,
                                     WN* wn_call,
                                     INT formal_number,
                                     INT idx_formal);

extern BOOL Array_Shapes_Match_At_Formal(IPA_LNO_READ_FILE* IPA_LNO_File,
                                         WN* wn_call,
                                         INT formal_number,
					 PROJECTED_REGION* pr_formal);

extern BOOL Shape_Mismatch_At_Common(IPA_LNO_READ_FILE* IPA_LNO_File,
                                     INT idx_common);

#endif /* ipa_lno_reshape_INCLUDED */
