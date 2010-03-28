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


/**
*** Implementation of external functions from opcode.h.
**/

/** $Revision$
*** $Date$
*** $Author$
*** $Source$
**/

#include "opcode.h"

#define opcode_C      "opcode.c"


/**
*** Looks up the name of this operator in the table from opcode_gen.c
**/

const char *OPERATOR_name(OPERATOR opr)
{
  Is_True(opr >= OPERATOR_FIRST && opr <= OPERATOR_LAST,
	  ("Bad OPERATOR %d", opr));

  return (const char *) OPERATOR_info[opr]._name;
}

/**
*** To make this lookup routine routine simple but efficient, we use a closed
*** hashing scheme.
**/

BOOL Operator_To_Opcode_Table_Inited = FALSE;
void Init_Operator_To_Opcode_Table(void)
{
    Operator_To_Opcode_Table_Inited = TRUE;
}


/* ====================================================================
 *
 * OPCODE OPCODE_commutative_op(OPCODE opc)
 *
 * If opc is commutative, return the opcode for whatever operation
 * gives equivalent results. If the operator isn't commutative, return 0.
 *
 * ====================================================================
 */

OPCODE OPCODE_commutative_op( OPCODE opc )
{
   
   OPCODE rop = (OPCODE) 0;
   OPERATOR opr = OPCODE_operator(opc);
   TYPE_ID rtype = OPCODE_rtype(opc);
   TYPE_ID desc = OPCODE_desc(opc);
   
   switch (opr) {
      /* These ops are commutative and don't need to be altered */
    case OPR_ADD:
    case OPR_MPY:
    case OPR_MAX:
    case OPR_MIN:
    case OPR_BAND:
    case OPR_BIOR:
    case OPR_BNOR:
    case OPR_BXOR:
    case OPR_LAND:
    case OPR_LIOR:
    case OPR_EQ:
    case OPR_NE:
      rop = opc;
      break;

      /* these are treated specially */
    case OPR_GT:
      rop = OPCODE_make_op(OPR_LT, rtype, desc);
      break;
    case OPR_GE:
      rop = OPCODE_make_op(OPR_LE, rtype, desc);
      break;
    case OPR_LT:
      rop = OPCODE_make_op(OPR_GT, rtype, desc);
      break;
    case OPR_LE:
      rop = OPCODE_make_op(OPR_GE, rtype, desc);
      break;

      /* Anything else is a null */
    default:
      break;
   }

   return (rop);
}
