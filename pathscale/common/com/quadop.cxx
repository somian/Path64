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



static char *source_file = __FILE__;
static char *rcs_id = "$Source$ $Revision$";
#include "defs.h"
#include "erglob.h"
#include "config_host.h"
#include "targ_const.h"

/*
   TODO -- reconcile the two quad types (QUAD_TYPE and quad).  These
   functions really are defined as using the structure quad, but it's
   irrelevant, as long as you don't look at the bits...  quad is
   defined to be a structure of two doubles, while QUAD_TYPE is a
   structure of four ints...  so, this is a hack, just to get this
   to compile (I really should have included quad.h from common/tdt/MIPS,
   and done something to reconcile the two kinds of quad types).
*/

INT __c_q_to_a(char*, QUAD_TYPE, INT*);
double __c_dble_q(QUAD_TYPE, INT*);
float __c_sngl_q(QUAD_TYPE, INT*);
INT32 __c_ji_qint(QUAD_TYPE, INT*);
UINT32 __c_ji_quint(QUAD_TYPE, INT*);
INT64 __c_ki_qint(QUAD_TYPE, INT*);
UINT64 __c_ki_quint(QUAD_TYPE, INT*);
QUAD_TYPE __c_a_to_q(char*, INT*);
QUAD_TYPE __c_q_extd(double, INT*);
QUAD_TYPE __c_q_ext(float, INT *);
QUAD_TYPE __c_q_flotj(INT32, INT*);
QUAD_TYPE __c_q_flotju(UINT32, INT*);
QUAD_TYPE __c_q_flotk(INT64, INT*);
QUAD_TYPE __c_q_flotku(UINT64, INT*);
QUAD_TYPE __c_q_add(QUAD_TYPE, QUAD_TYPE, INT*);
QUAD_TYPE __c_q_sub(QUAD_TYPE, QUAD_TYPE, INT*);
QUAD_TYPE __c_q_mul(QUAD_TYPE, QUAD_TYPE, INT*);
QUAD_TYPE __c_q_div(QUAD_TYPE, QUAD_TYPE, INT*);
QUAD_TYPE __c_q_sqrt(QUAD_TYPE, INT*);
QUAD_TYPE __c_q_neg(QUAD_TYPE, INT*);
INT __c_q_eq(QUAD_TYPE, QUAD_TYPE, INT*);
INT __c_q_ne(QUAD_TYPE, QUAD_TYPE, INT*);
INT __c_q_lt(QUAD_TYPE, QUAD_TYPE, INT*);
INT __c_q_le(QUAD_TYPE, QUAD_TYPE, INT*);
INT __c_q_gt(QUAD_TYPE, QUAD_TYPE, INT*);
INT __c_q_ge(QUAD_TYPE, QUAD_TYPE, INT*);

/* Currently only a stub file. */
QUAD_TYPE My_Atoq(char *str)
{
 INT err;
 char *sstr;

 for ( sstr = str; *sstr; sstr++ ) {
  if (*sstr == 'q' || *sstr == 'Q') {
   *sstr='E';
   break;
  }
 }
 return (__c_a_to_q(str,&err));
}

INT QUADOP_EQ(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_eq(a,b,&err));
}

INT QUADOP_NE(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_ne(a,b,&err));
}

INT QUADOP_GE(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_ge(a,b,&err));
}

INT QUADOP_GT(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_gt(a,b,&err));
}

INT QUADOP_LE(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_le(a,b,&err));
}

INT QUADOP_LT(QUAD_TYPE a, QUAD_TYPE b)
{
 INT err;
 return(__c_q_lt(a,b,&err));
}

QUAD_TYPE QUADOP_ADD(QUAD_TYPE a,QUAD_TYPE b)
{
 INT err;
 return(__c_q_add(a,b,&err));
}

QUAD_TYPE QUADOP_SUB(QUAD_TYPE a,QUAD_TYPE b)
{
 INT err;
 return(__c_q_sub(a,b,&err));
}

QUAD_TYPE QUADOP_MPY(QUAD_TYPE a,QUAD_TYPE b)
{
 INT err;
 return(__c_q_mul(a,b,&err));
}

QUAD_TYPE QUADOP_DIV(QUAD_TYPE a,QUAD_TYPE b)
{
 INT err;
 return(__c_q_div(a,b,&err));
}

QUAD_TYPE QUADOP_NEGATE(QUAD_TYPE a)
{
 INT err;
 return(__c_q_neg(a,&err));
}

double QUADOP_QTOD (QUAD_TYPE a)
{
 INT err;
 return(__c_dble_q(a,&err));
}

float QUADOP_QTOS (QUAD_TYPE a)
{
 INT err;
 return(__c_sngl_q(a,&err));
}

INT QUADOP_QTOI (QUAD_TYPE a)
{
 INT err;
 return(__c_ji_qint(a,&err));
}

UINT QUADOP_QTOU (QUAD_TYPE a)
{
 INT err;
 return(__c_ji_quint(a,&err));
}

QUAD_TYPE QUADOP_UTOQ (UINT a)
{
 INT err;
 return(__c_q_flotju(a,&err));
}

QUAD_TYPE QUADOP_STOQ (float a)
{
 INT err;
 return(__c_q_ext(a,&err));
}

QUAD_TYPE QUADOP_DTOQ (double a)
{
 INT err;
 return(__c_q_extd(a,&err));
}

QUAD_TYPE QUADOP_ITOQ (INT a)
{
 INT err;
 return(__c_q_flotj(a,&err));
}

QUAD_TYPE QUADOP_SQRT(QUAD_TYPE a)
{
 INT err;
 return(__c_q_sqrt(a,&err));
}

