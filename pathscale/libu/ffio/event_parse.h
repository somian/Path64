/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


/* USMID @(#) libu/ffio/event_parse.h	92.1	10/07/99 22:14:06 */

/*  EVENT LAYER ASSIGN PARSING DEFINITION */

#include "evntio.h"
#define NUM_EVENT_ALIAS    0
#define NUM_EVENT_NUMERICS 0

/* The undocumented "trace" parameter is accepted on PVP systems. */
/* It has been informally supported there, so that internal people could */
/* use application's tools for examining it. I have no idea whether */
/* it still works there. This tool isn't available on the T3E. It is */
/* available on Irix, but the format of the trace file has changed - so */
/* if we ever support "trace" there, changes will be required (7/17/97) */
struct LAYER_OPTS _event_opts[] = {
 CLASS_EVENT, _STR1M, _INFO1_STR1(TR_EVNT_NOSTAT),   0, 0, _STR1M, 0, 0, "nostat" ,
 CLASS_EVENT, _STR1M, _INFO1_STR1(TR_EVNT_SUMMARY),  0, 0, _STR1M, 0, 0, "summary" ,
 CLASS_EVENT, _STR1M, _INFO1_STR1(TR_EVNT_BRIEF),    0, 0, _STR1M, 0, 0, "brief",
#if !defined(__mips) && !defined(_CRAYT3E) && !defined(_LITTLE_ENDIAN)
 CLASS_EVENT, _STR2M, _INFO1_STR2(TR_EVNT_TRACE),    0, 0, _STR2M, 0, 0, "trace"
#endif
} ;


#define NUM_EVENT_OPTS     (sizeof(_event_opts)/sizeof(struct LAYER_OPTS))

struct LAYER_DATA _event_data =
    {
         CLASS_EVENT ,
         FLD_TYPE,
         "event",
         "" ,
         0,
         0,
         NUM_EVENT_OPTS,
         1 ,
         NUM_EVENT_NUMERICS,
         NUM_EVENT_ALIAS ,
         _event_opts ,
	 NULL,
	 NULL,
         NULL
    } ;

