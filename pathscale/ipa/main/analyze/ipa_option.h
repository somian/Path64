/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

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
 * Module: ipa_option.h
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:40-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/main/analyze/SCCS/s.ipa_option.h $
 *
 * Revision history:
 *  08-Jun-95 - Original Version
 *  11-Apr-96 - Removed most option variables to common/com/config_ipa.
 *
 * Description:
 *  List of global variables set by the -IPA option group.
 *  See also common/com/config_ipa.h.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipa_option_INCLUDED
#define ipa_option_INCLUDED

#include "config_ipa.h"		/* For -INLINE/-IPA group options */

#ifndef ipa_trace_INCLUDED
#include "ipa_trace.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LIGHTWEIGHT_INLINER
#ifndef __DEM_H__
#include "ld_dem.h"		    /* for C++ demangler */
#endif
#endif // _LIGHTWEIGHT_INLINER

#ifndef ipc_weak_INCLUDED
#include "ipc_weak.h"		    /* make demangle() weak extern */
#endif

struct skiplist;

extern BOOL IPA_Enable_Memtrace;    /* Memory trace */

extern struct skiplist *IPA_Skip_List;	/* List of skip options */

extern BOOL Trace_IPA;		    /* Main IPA progress trace */
extern BOOL Trace_Perf;		    /* performance tracing */
extern BOOL Trace_IPALNO;         /* trace IPA to LNO writing */ 

extern BOOL Verbose;
extern BOOL Demangle;		    /* demangle C++ identifiers in error msg */

extern BOOL ProMP_Listing;

#ifdef KEY
extern INT32 OPT_Cyg_Instrument;     /* Limits DFE if on */
#endif

#undef DEMANGLE
#define DEMANGLE(name) (name)
#if 0
#if (defined(_STANDALONE_INLINER) || defined(_LIGHTWEIGHT_INLINER))
#define DEMANGLE(name) (name)
#else
#define DEMANGLE(name) (always_demangle (name, (char) Demangle))
#endif /* __STANDALONE_INLINER */
#endif
    
extern void Process_IPA_Options ( INT argc, char **argv );

#ifdef __cplusplus
}
#endif

#endif /* ipa_option_INCLUDED */
