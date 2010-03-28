/*
 * Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
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


//-*-c++-*-
/* ====================================================================
 * ====================================================================
 *
 * Module: ipo_tlog_util.h
 * $Revision: 1.4 $
 * $Date: 04/12/21 14:57:42-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/main/optimize/SCCS/s.ipo_tlog_utils.h $
 *
 * Description: Defines tlog utilities for IPA, originated from OPT
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipo_tlog_util_INCLUDED
#define ipo_tlog_util_INCLUDED	"ipo_tlog_util.h"
#ifdef _KEEP_RCS_ID
static char *rcs_id = 	ipo_tlog_util_INCLUDE"$Revision: 1.4 $";
#endif /* _KEEP_RCS_ID */

#ifndef srcpos_INCLUDED
#include "srcpos.h"
#endif /* srcpos_INCLUDED */
#ifndef KEY
enum PHASE_NAME {PHASE_INLINER, PHASE_IPA};
#else
enum PHASE_NAME {PHASE_INLINER, PHASE_IPA, INVALID};
extern "C" PHASE_NAME Get_ipa_tlog_phase(void);
#endif // !KEY
extern "C" void Set_ipa_tlog_phase(const INT32 phase); 

// TLOG interface for reporting optimizations
extern "C" void Ipa_tlog( const char *keyword, SRCPOS srcpos, const char *fmt, ... );

extern "C" void Inline_tlog( const char *keyword, SRCPOS srcpos, const char *fmt, ... );

#endif // ipo_tlog_util_INCLUDED

