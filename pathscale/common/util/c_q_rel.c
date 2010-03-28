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
 *  Module: c_q_rel.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 * =======================================================================
 * =======================================================================
 */

static char *source_file = __FILE__;
static char *rcs_id = "$Source$ $Revision$";


#include "defs.h"
#include "quad.h"

extern INT c_q_lt(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_lt(QUAD x, QUAD y, INT *p_err) { return c_q_lt(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_lt = __c_q_lt
#define	c_q_lt __c_q_lt
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator < for long doubles */

INT
c_q_lt(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi < y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo < y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_le(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_le(QUAD x, QUAD y, INT *p_err) { return c_q_le(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_le = __c_q_le
#define	c_q_le __c_q_le
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator <= for long doubles */

INT
c_q_le(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi < y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo <= y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_eq(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_eq(QUAD x, QUAD y, INT *p_err) { return c_q_eq(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_eq = __c_q_eq
#define	c_q_eq __c_q_eq
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator == for long doubles */

INT
c_q_eq(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( (x.hi == y.hi) && (x.lo == y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_ne(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_ne(QUAD x, QUAD y, INT *p_err) { return c_q_ne(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_ne = __c_q_ne
#define	c_q_ne __c_q_ne
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator != for long doubles */

INT
c_q_ne(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( (x.hi != y.hi) || (x.lo != y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_gt(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_gt(QUAD x, QUAD y, INT *p_err) { return c_q_gt(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_gt = __c_q_gt
#define	c_q_gt __c_q_gt
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator > for long doubles */

INT
c_q_gt(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi > y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo > y.lo) )
		return ( 1 );

	return ( 0 );
}

extern INT c_q_ge(QUAD, QUAD, INT *);
#if defined(BUILD_OS_DARWIN)
/* Can't use "pragma weak" to create aliases in Mach-O */
extern INT __c_q_ge(QUAD x, QUAD y, INT *p_err) { return c_q_ge(x, y, p_err); }
#else /* defined(BUILD_OS_DARWIN) */
#pragma weak c_q_ge = __c_q_ge
#define	c_q_ge __c_q_ge
#endif /* defined(BUILD_OS_DARWIN) */

	/* implements the relational operator >= for long doubles */

INT
c_q_ge(QUAD x, QUAD y, INT *p_err )
{
	*p_err = 0;

	if ( x.hi > y.hi )
		return ( 1 );

	if ( (x.hi == y.hi) && (x.lo >= y.lo) )
		return ( 1 );

	return ( 0 );
}

