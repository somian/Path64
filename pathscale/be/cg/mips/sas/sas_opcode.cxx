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

#include "comm.h"
#include "sas_opcode.h"


static const char* const opcode_names[] = {
#define NO_OPX
#define op(a,b,c,d,e,f,g)  c,
#include "sas_op.h"
};


const char* Opcode_Name( const STOP top )
{
  return opcode_names[(int)top];
}


STOP Find_Opcode( const char* src )
{
  static const int entries = sizeof(opcode_names) / sizeof(opcode_names[0]);

  for( int i = 0; i < entries; i++ ){
    if( strcmp( src, opcode_names[i] ) == 0 ){
      return (STOP)i;
    }
  }

  return STOP_unknown;
}


const uint64_t OPR_ATTR_flags[] = {
#define NO_OPX
#define op(a,b,c,d,e,f,g) f,
#include "sas_op.h"
};

const uint8_t OPR_throughput[] = {
#define NO_OPX
#define op(a,b,c,d,e,f,g) g,
#include "sas_op.h"
};
