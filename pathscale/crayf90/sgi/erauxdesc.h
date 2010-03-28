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



#ifndef erauxdesc_INCLUDED
#define erauxdesc_INCLUDED

#include "erfe90.h"

ERROR_DESC EDESC_FE[] = {
  { EC_Multiple_Initialization,	EM_User | ES_WARNING,	RAG_EN_NONE,
    "Multiple DATA initialization of storage for %s. Some initializations ignored.",
    1, ET_STRING, 0,0,0,0,0 },
  { EC_Makedepend_Error,	EM_User | ES_ERRABORT,	RAG_EN_NONE,
    "Error making make dependency file: %s",
    1, ET_STRING, 0,0,0,0,0 },
  { EC_Unknown_Mpsched,	        EM_User | ES_WARNING,	RAG_EN_NONE,
    "Unknown value for -mp_schedtype: %s",
    1, ET_STRING, 0,0,0,0,0 },
  { EC_IEEE_Intrinsic_Warning,	EM_User | ES_WARNING,	RAG_EN_NONE,
    "Optimization may move floating-point computations relative\nto the %s intrinsic, possibly resulting in different behavior.",
    1, ET_STRING, 0,0,0,0,0 },

  /* All error descriptor lists must end with a -1 error code: */
  { -1,	0, RAG_EN_NONE, "", 0, 0,0,0,0,0,0 }

};


#endif
