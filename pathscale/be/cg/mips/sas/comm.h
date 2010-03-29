/*
 * Copyright 2005, 2007 PathScale Inc.  All Rights Reserved.
 *

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

#ifndef COMM_H
#define COMM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/unistd.h>
#include <sys/times.h>
#include <sys/param.h>
#include "mempool.h"

extern MEM_POOL sas_pool;

#define ASSERT(f)                                           \
  do{                                                       \
    if(!(f) ){                                              \
      fprintf( stderr, "SCHED: [%s] ", #f );                \
      fprintf( stderr, "at %s %d\n", __FILE__, __LINE__ );  \
      My_Abort();                                           \
    }                                                       \
  } while(0)


#define Ceiling_Divide(x,y)  ( (int) ( ((x) + (y) - 1) / (y) ) )

// We use the following levels to control the debugging info.
typedef enum {
  MUTE,     // provide no info
  TACITURN, // provide info only for critical steps
  LACONIC,  // provide info after an object is finished
  VOLUBLE   // provide info for each step at each object
} TALK;
#endif
