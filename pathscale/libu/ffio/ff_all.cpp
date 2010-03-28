
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

/* USMID @(#) libu/ffio/ff_all.cpp	92.0	10/08/98 14:57:41 */


#include <fdcconfig.h>
#include "fxlist.h"

*
* IBM, VMS, NOSVE and other F and V class
* record blocking routines
*
LOADIT(F_XLIST_P)
LOADIT(V_XLIST_P)
#ifndef _CRAYIEEE
*
* IBM numerics
*
HARDREF=CRAY2IBM,IBM2CRAY
#endif

#ifndef _CRAYIEEE
*
* VAX/VMS numerics
*
HARDREF=CRAY2VAX,VAX2CRAY
#endif

*
* NOS/VE, f77 and ETA blocking routines
*
LOADIT(X_XLIST_P)

#ifndef _CRAYIEEE
*
* NOS/VE numerics
*
HARDREF=CRAY2NVE,NVE2CRAY
#endif

#ifndef _CRAYIEEE
*
* ETA numerics
*
HARDREF=CRAY2ETA,ETA2CRAY
#endif

#if defined(_CRAYMPP) || !defined(_CRAYIEEE)
*
* IEEE numerics
*
HARDREF=CRAY2IEG,IEG2CRAY
#endif

*
* COS blocking routines
*
LOADIT(COS_XLIST_P)

#ifndef _CRAYMPP
*
* BMX/TAPE routines
*
LOADIT(TAPE_XLIST_P)
#endif

#ifndef _CRAYIEEE
*
* CDC routines
*
LOADIT(CDC_XLIST_P)
HARDREF=CRAY2CDC,CDC2CRAY
#endif

#if defined(_CRAY1) || (defined(_CRAYMPP) && defined(_UNICOS_MAX))
*
* SDS routines
*
LOADIT(SDS_XLIST_P)
#endif

#ifdef  _CRAY1
*
* ER90 byte stream routines
*
LOADIT(ER90B_XLIST_P)
#endif

*
* MR routines
*
LOADIT(MR_XLIST_P)

*
* TRACE routines
*
LOADIT(TRC_XLIST_P)

*
* TEXT routines
*
LOADIT(TXT_XLIST_P)

*
* USER routines are *always* soft
*
****LOADIT(USR_XLIST_P)

*
* SITE routines
*
#if ENABLE_SITE_LYR
LOADIT(SITE_XLIST_P)
#else
* layer not enabled
#endif
#if ENABLE_SITE_DAT
HARDREF=CRAY2STE,STE2CRAY
#else
* data conversion not enabled
#endif

*
* FD routines
*
LOADIT(FD_XLIST_P)

*
* BLX routines
*
LOADIT(BLX_XLIST_P)

*
* CACHE routines
*
LOADIT(CCH_XLIST_P)

*
* CACHEA routines
*
LOADIT(CCA_XLIST_P)

*
* EVENT routines
*
LOADIT(EVNT_XLIST_P)

#if defined(_CRAYMPP) 
*
* GLOBAL routines
*
LOADIT(GLOBAL_XLIST_P)
#endif
