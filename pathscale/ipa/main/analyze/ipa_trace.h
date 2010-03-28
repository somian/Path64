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


#ifndef ipa_trace_INCLUDED
#define ipa_trace_INCLUDED

/* Trace Options -- Please put them here, and not in arbitrary places */

#define IPA_TRACE_IPA                    1
#define IPA_TRACE_IPAA                   2
#define IPA_TRACE_DETAIL                 4
#define IPA_TRACE_CG                     8
#define IPA_TRACE_IPAA_SUMMARY          16
#define IPA_TRACE_STATS                 32
#define IPA_TRACE_ITERATOR              64
#define IPA_TRACE_SPLIT_COMMON         128
#define IPA_TRACE_SECTIONS             256
#define IPA_TRACE_CPROP_CLONING        512
#define IPA_TRACE_LNO_WRITE           1024
#define IPA_TRACE_SECTION_CORRECTNESS 2048
#define IPA_TRACE_PREOPT_IPL          4096
#define IPA_TRACE_MODREF              8192
#define IPA_TRACE_COMMON_CONST       16384
#define IPA_TRACE_RESHAPE            32768
#define IPA_TRACE_EXCOST             65536
#define IPA_TRACE_SIMPLIFY          131072
#define IPA_TRACE_TUNING           0x40000
#define IPA_TRACE_TUNING_NEW       0x80000

#endif /* ipa_trace_INCLUDED */
