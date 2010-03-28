/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* ====================================================================
 * ====================================================================
 *
 * Module: ipc_option.h
 * $Revision: 1.4 $
 * $Date: 04/12/21 14:57:37-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/common/SCCS/s.ipc_option.h $
 *
 * Revision history:
 *  31-Jul-95 - Original Version
 *
 * Description:
 *
 * Flags and routines for processing common INLINE/IPA options.
 * See also common/com/config_ipa.[hc].
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipc_option_INCLUDED
#define ipc_option_INCLUDED

#ifndef strtab_INCLUDED
#include "strtab.h"             // STRTAB_TYPE
#endif

#include <ext/hash_map>

#ifndef mempool_allocator_INCLUDED
#include "mempool_allocator.h"
#endif

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

struct eqint
{
  bool operator()(const INT s1, const INT s2) const
  {
    return (s1 == s2);
  }
};

typedef  __gnu_cxx::hash_map<const char*, UINT, __gnu_cxx::hash<const char*>, eqstr > INLINE_PU_MAP;
typedef  __gnu_cxx::hash_map<const INT, UINT, __gnu_cxx::hash<INT> > INLINE_EDGE_MAP;

extern UINT User_Specified_Name_Info(char *);
extern UINT User_Specified_Edge_Info(INT);
extern BOOL Is_User_Must_Inline(UINT);
extern BOOL Is_User_No_Inline(UINT);
extern BOOL Is_User_Not_Specified(UINT);
extern BOOL Is_Skip_Not_Specified(char * );
extern BOOL Is_Skip_Equal(char * );

#ifdef _STANDALONE_INLINER
extern void Process_Non_Local_Files();
extern void Process_Non_Local_Libraries();
#endif // _STANDALONE_INLINER


extern INT number_of_partitions;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void Process_Inline_Options ( void );
extern void Process_IPA_Specfile_Options( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* ipc_option_INCLUDED */
