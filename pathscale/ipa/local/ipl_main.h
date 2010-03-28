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


#ifndef ipl_main_INCLUDED
#define ipl_main_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* General progress trace: */
extern BOOL Trace_IPA;
extern BOOL Trace_Perf;
extern BOOL Debug_On;
extern BOOL Do_Const;
extern BOOL Do_Par;
extern BOOL Do_Split_Commons;
extern BOOL Do_Split_Commons_Set;
extern BOOL IPL_Enable_Unknown_Frequency;
extern BOOL IPL_Generate_Elf_Symtab;    
#ifdef KEY
extern UINT32 IPL_Ignore_Small_Loops;
#endif
extern struct DU_MANAGER *Ipl_Du_Mgr;
extern struct ALIAS_MANAGER *Ipl_Al_Mgr;

#ifdef __cplusplus
}
#endif

extern BOOL DoPreopt;

extern WN_MAP Summary_Map;
extern WN_MAP Stmt_Map;

#endif // ipl_main_INCLUDED
