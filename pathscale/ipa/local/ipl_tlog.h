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
 * Module: ipl_tlog.h
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:39-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/local/SCCS/s.ipl_tlog.h $
 *
 * Description: Defines tlog utilities for IPL, originated from OPT
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipl_tlog_INCLUDED
#define ipl_tlog_INCLUDED  "ipl_tlog.h"

#ifndef srcpos_INCLUDED
#include "srcpos.h"
#endif /* srcpos_INCLUDED */
// TLOG interface
extern "C" void Ipl_record_tlog( const char *keyword, SRCPOS srcpos, const char *fmt, ... );
#endif // ipl_tlog_INCLUDED
