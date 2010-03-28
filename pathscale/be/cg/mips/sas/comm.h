/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2005 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
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
