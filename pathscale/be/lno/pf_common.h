/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifndef pf_common_INCLUDED
#define pf_common_INCLUDED

extern MEM_POOL *PF_mpool;
extern BOOL Debug_Prefetch;
extern FILE* TFile;
extern BOOL Verbose_Prefetch;

#ifdef  PF_PrintDebugging
#define PF_PRINT(x) if (Debug_Prefetch) { x; }
#else
#define PF_PRINT(x)
#endif

#ifdef Is_True_On
#define VB_PRINT(x) if (Verbose_Prefetch) { x; }
#else
#define VB_PRINT(x)
#endif

/* in verbose mode, the pretty printing indent */
extern UINT vb_num_indent;
extern BOOL vb_print_split;
#define vb_print_indent { for (INT i=0;i<vb_num_indent; i++) printf (" "); }

/* in list mode, the pretty printing indent */
extern UINT ls_num_indent;
#define ls_print_indent { for (INT i=0;i<ls_num_indent; i++) fprintf (LNO_Analysis, " "); }
extern void dump_tree (WN *wn);
extern void dump_wn (WN *wn);

#define PF_MAX_STRIDE 33

#endif // pf_common_INCLUDED
