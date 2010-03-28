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


#ifndef wb_ipl_INCLUDED
#define wb_ipl_INCLUDED

#ifndef ipl_summarize_INCLUDED
class SUMMARY; 
#endif 

#ifndef ipa_section_INCLUDED
class ARRAY_SUMMARY;
#endif

extern void WB_IPL_Save();

extern void WB_IPL_Initialize(WN* wn_global, DU_MANAGER* du, ALIAS_MANAGER* am);

extern void WB_IPL_Set_Access_Array_Map(WN_MAP access_array_map);

extern void WB_IPL_Set_Reduction_Map(WN_MAP reduction_map);

extern void WB_IPL_Set_Scalar_Summary(SUMMARY* scalar_summary);

extern void WB_IPL_Set_Array_Summary(ARRAY_SUMMARY* array_summary);

extern void WB_IPL_Terminate();

extern void WB_IPL_Restore();

extern void s_ipl_debug(const char init_buffer[]);

#endif /* wb_ipl_INCLUDED */
