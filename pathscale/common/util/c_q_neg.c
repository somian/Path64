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


/* =======================================================================
 * =======================================================================
 *
 *  Module: c_q_neg.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 * =======================================================================
 * =======================================================================
 */


#include "defs.h"
#include "quad.h"

/* quad unary minus */

#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
QUAD c_q_neg(QUAD x, INT *p_err );
QUAD __c_q_neg(QUAD x, INT *p_err ) { return c_q_neg(x, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
extern QUAD c_q_neg(QUAD, INT *);
#pragma weak c_q_neg = __c_q_neg
#define	c_q_neg __c_q_neg
#endif /* defined(BUILD_OS_DARWIN) */

QUAD
c_q_neg(QUAD x, INT *p_err )
{
QUAD	result;

	*p_err = 0;

	result.hi = -(x.hi);
	result.lo = -(x.lo);

	return ( result );
}

