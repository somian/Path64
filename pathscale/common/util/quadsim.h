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


#ifndef quadsim_INCLUDED
#define quadsim_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* =======================================================================
 * =======================================================================
 *
 *  Module: quadsim.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 * =======================================================================
 * =======================================================================
 */


extern INT32 __c_fp_class_q(QUAD x);
extern INT __c_q_to_a(char*, QUAD, INT*);
extern double __c_dble_q(QUAD, INT*);
extern float __c_sngl_q(QUAD, INT*);
extern INT32 __c_ji_qint(QUAD, INT*);
extern UINT32 __c_ji_quint(QUAD, INT*);
extern INT64 __c_ki_qint(QUAD, INT*);
extern UINT64 __c_ki_quint(QUAD, INT*);
extern QUAD __c_a_to_q(char*, INT*);
extern QUAD __c_q_extd(double, INT*);
extern QUAD __c_q_ext(float, INT *);
extern QUAD __c_q_flotj(INT32, INT*);
extern QUAD __c_q_flotju(UINT32, INT*);
extern QUAD __c_q_flotk(INT64, INT*);
extern QUAD __c_q_flotku(UINT64, INT*);
extern QUAD __c_q_add(QUAD, QUAD, INT*);
extern QUAD __c_q_sub(QUAD, QUAD, INT*);
extern QUAD __c_q_mul(QUAD, QUAD, INT*);
extern QUAD __c_q_div(QUAD, QUAD, INT*);
extern QUAD __c_q_sqrt(QUAD, INT*);
extern QUAD __c_q_neg(QUAD, INT*);
extern QUAD __c_q_trunc(QUAD, INT*);
extern INT __c_q_eq(QUAD, QUAD, INT*);
extern INT __c_q_ne(QUAD, QUAD, INT*);
extern INT __c_q_lt(QUAD, QUAD, INT*);
extern INT __c_q_le(QUAD, QUAD, INT*);
extern INT __c_q_gt(QUAD, QUAD, INT*);
extern INT __c_q_ge(QUAD, QUAD, INT*);

#ifdef __cplusplus
}
#endif
#endif /* quadsim_INCLUDED */
