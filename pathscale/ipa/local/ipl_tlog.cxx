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
 * Module: ipl_tlog.cxx
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:38-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/local/SCCS/s.ipl_tlog.cxx $
 *
 * Description: Defines tlog utilities for IPL, originated from OPT
 *
 * ====================================================================
 * ====================================================================
 */
// ====================================================================
// Usage:   -Wi,-tt1:18 -keep : generates file.tlog for IPL
// ====================================================================
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>

#ifndef USE_STANDARD_TYPES
#define USE_STANDARD_TYPES
#endif
#include "defs.h"
#include "errors.h"
#include "tracing.h"
#include "srcpos.h"
#include "tlog.h"
#include "ipl_tlog.h"
const INT32 MAX_WARN_LEN = 1024;

static const char* tlog_phase = "IPL";

static BOOL
Ipl_tlog_trace( void )
{
  return Get_Trace ( TP_PTRACE1, TP_IPL );
}

static  
void Ipl_tlog2(const char *keyword, INT64 srcpos, const char *msg )
{
  // use the keyword as both the transformation name and keyword
  Generate_Tlog( tlog_phase, keyword, (SRCPOS)srcpos, keyword, msg, "","" );
}

static void 
Ipl_tlog(const char *keyword, INT64 srcpos, const char *fmt,va_list vp)
{
  char msg_buf[MAX_WARN_LEN];
  INT32 len;
  vsprintf(msg_buf, fmt, vp);     // if msg is too long, it might overrun the buf
  len = strlen(msg_buf);
  //  FmtAssert doesn't work here!
  if (len >= MAX_WARN_LEN) {
    fprintf(stderr, "Ipa_tlog message buffer too small.");
  }

  Ipl_tlog2( keyword, srcpos, msg_buf );
}




// ====================================================================
// TLOG external interface for reporting optimizations
// ====================================================================
extern "C" void 
Ipl_record_tlog(const char *keyword, SRCPOS srcpos, const char *fmt, ...)
{
  va_list ap; 
  va_start(ap, fmt);
  if ( ! Ipl_tlog_trace() ) {
    return;
  }
  Ipl_tlog(keyword, srcpos, fmt, ap);
  va_end(ap);
}
