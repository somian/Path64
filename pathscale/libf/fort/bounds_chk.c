/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fort/bounds_chk.c	92.1	06/24/99 10:18:36"
#include <fortran.h>
#include <liberrno.h>
#include <stddef.h>
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
#include <stdlib.h>
#endif
#include "fio.h"

#ifdef KEY /* Bug 7969 */
/* return true if environment says to abort after bounds/subscript error */
static int
want_abort() {
  static int want_abort_var = -1;
  if (want_abort_var == -1) {
    char *abort_now = getenv("F90_BOUNDS_CHECK_ABORT");
    want_abort_var = (abort_now && (*abort_now == 'y' || *abort_now == 'Y'));
  }
  return want_abort_var;
}

/* Do cleanup and abort after issuing bounds/subscript error */
static void
do_abort() {
  _fcleanup();
  abort();
}
#endif /* KEY Bug 7969 */

/*
 *  Error handler for an of out-of-bounds subscript warning.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input Arguments:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of array which had an out-of-bounds subscript.
 *    dim      - Dimension number of the array.
 *    lowerbnd - Lower bound of dimension dim
 *    upperbnd - Upper bound of dimension dim
 *    sub      - Out-of-bounds subscript value.
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */

void
_BOUNDS_ERROR(
	char	*file,		/* Fortran routine containing error */
	int	*line,		/* Line number in Fortran routine */
	char	*variable,	/* arrayname with out-of-bounds subscript */
	int	*dim,		/* Dimension number of the array */
	int	*lowerbnd,	/* Lower bound of dimension dim */
	int	*upperbnd,	/* Upper bound of dimension dim */
	int	sub[1],		/* Out-of-bounds subscript value */
	int	*count)		/* Count/flag for number of messages */
{
	int	*retcnt;	/* ptr to static arg count word */
	int	intcnt = 0;	/* local count if no count passed */
#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 8)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;
	if ((*retcnt)++ == 0) {
#ifdef KEY /* Bug 7969 */
		if (want_abort()) {
		  (void) _lerror(_LELVL_MSG, FWARGSBV, sub[0], *dim, variable,
		     *line, file, *lowerbnd, *upperbnd);
		  do_abort();
		}
#endif /* KEY Bug 7969 */
		(void) _fwarn(FWARGSBV, sub[0], *dim, variable, *line,
		   file, *lowerbnd, *upperbnd);
	}
	return;
}

/*
 *  Error handler for a range of out-of-bounds subscripts.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of array which had an out-of-bounds subscript.
 *    dim      - Dimension number of the array.
 *    lowerbnd - Lower bound of dimension dim
 *    upperbnd - Upper bound of dimension dim
 *    start    - Starting subscript which is out of bounds.
 *    end      - Ending subscript which is out of bounds.
 *    incr     - Increment between subscripts.
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */

void
_RBOUNDS_ERROR(
	char	*file,		/* Fortran routine containing error */
	int	*line,		/* Line number in Fortran routine */
	char	*variable,	/* arrayname with out-of-bounds subscript */
	int	*dim,		/* Dimension number with size mismatch */
	int	*lowerbnd,	/* Lower bound of dimension dim */
	int	*upperbnd,	/* Upper bound of dimension dim */
	int	*start,		/* Out-of-bounds starting subscript */
	int	*end,		/* Out-of-bounds ending subscript */
	int	*incr,		/* Increment between subscript */
	int	*count)		/* Count/flag for number of messages */
{
	int	*retcnt;	/* ptr to static arg count word */
	int	intcnt = 0;	/* local count if no count passed */
#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 10)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;
	if ((*retcnt)++ == 0) {
#ifdef KEY /* Bug 7969 */
		if (want_abort()) {
		  (void) _lerror(_LELVL_MSG, FWARGSBR, *start, *end, *incr,
		     *dim, variable, *line, file, *lowerbnd, *upperbnd);
		  do_abort();
		}
#endif /* KEY Bug 7969 */
		(void) _fwarn(FWARGSBR, *start, *end, *incr, *dim,
			variable, *line, file, *lowerbnd, *upperbnd);
	}
	return;
}

/*
 *  Error handler for a vector of out-of-bounds subscripts.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of array which had an out-of-bounds subscript.
 *    dim      - Dimension number of the array.
 *    lowerbnd - Lower bound of dimension dim
 *    upperbnd - Upper bound of dimension dim
 *    sub      - Array of subscripts.
 *    vm       - Bit mask of which subscripts are in error.
 *    vl       - Number of subscripts passed.
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */

void
_VBOUNDS_ERROR(
	char	*file,		/* Fortran routine with error */
	int	*line,		/* Line number in Fortran routine */
	char	*variable,	/* Arrayname with out-of-bounds subscript */
	int	*dim,		/* Dimension number of the array */
	int	*lowerbnd,	/* Lower bound of dimension dim */
	int	*upperbnd,	/* Upper bound of dimension dim */
	int	sub[128],	/* Array of subscripts */
	long	vm[2],		/* Bit mask of the subscripts in error */
	int	*vl,		/* Number of subscripts passed */
	int	*count)		/* Count/flag for number of messages */
{
	int 	i, len;
	long	mask;
	int 	stride;
	int 	first_error_sub;
	int 	last_error_sub;
	int 	constant_stride = 1;
	int 	error_count     = 0;
	int	*retcnt;	/* ptr to static arg count word */
	int	intcnt = 0;	/* local count if no count passed */
#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 10)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;
	if ((*retcnt)++ != 0)
		return;
	len	= *vl;
	for ( mask = vm[0], i = 0; i < len; i++, mask <<= 1 ) {
		if ( i == 64 ) {
			mask	= vm[1];
		}
		if ( mask < 0 ) {
			error_count++;
			if ( error_count == 1 ) {
				first_error_sub	= sub[i];
			}
			else if ( error_count == 2 ) {
				stride	= sub[i] - last_error_sub;
			}
			else {
				if ( stride != sub[i] - last_error_sub ) {
					constant_stride	= 0;
					break;
				}
			}
			last_error_sub	= sub[i];
		}
	}
	if ( error_count == 1 ) {
		(void) _fwarn(FWARGSBV, first_error_sub, *dim, variable,
				*line, file, *lowerbnd, *upperbnd);
		return;
	}
	else if ( constant_stride ) {
		if ( stride == 0 ) {
			(void) _fwarn(FWARGSBV, first_error_sub, *dim,
					variable, *line, file, *lowerbnd,
					*upperbnd);
		}
		else {
			(void) _fwarn(FWARGSBR, first_error_sub,
					last_error_sub, stride, *dim,
					variable, *line, file, *lowerbnd,
					*upperbnd);
		}
		return;
	}
	for ( mask = vm[0], i = 0; i < len; i++, mask <<= 1 ) {
		if ( i == 64 ) {
			mask = vm[1];
		}
		if ( mask < 0 ) {
			(void) _fwarn(FWARGSBV, sub[i], *dim, variable,
					*line, file, *lowerbnd, *upperbnd);
		}
	}
}


/*
 *  Error handler for a vector of out-of-bounds ranges.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of array which had an out-of-bounds subscript.
 *    dim      - Dimension number of the array.
 *    lowerbnd - Lower bound of dimension dim
 *    upperbnd - Upper bound of dimension dim
 *    start    - Array of starting subscripts
 *    end      - Array of ending subscripts
 *    incr     - Array of increments
 *    vm       - Bit mask of which subscripts are in error.
 *    vl       - Number of subscripts passed.
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */

void
_VRBOUNDS_ERROR(
	char    *file,		/* Fortran routine with error */
	int     *line,		/* Line number in Fortran routine */
	char    *variable,	/* Arrayname with out-of-bounds subscript */
	int     *dim,		/* Dimension number of the array */
	int     *lowerbnd,	/* Lower bound of dimension dim */
	int     *upperbnd,	/* Upper bound of dimension dim */
	int     start[128],	/* Out-of-bounds starting subscript */
	int     end[128],	/* Out-of-bounds ending subscript */
	int     incr[128],	/* Increment between subscript */
	long    vm[2],		/* Bit mask of the subscripts in error */
	int     *vl,		/* Number of subscripts passed */
	int	*count)		/* Count/flag for number of messages */
{
	int 	i, len;
	long	mask;
	int 	first_error_start;
	int 	first_error_end;
	int 	first_error_incr;
	int 	all_same    = 1;
	int 	first_error = 1;
	int	*retcnt;	/* ptr to static arg count word */
	int	intcnt = 0;	/* local count if no count passed */

#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 12)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;
	if ((*retcnt)++ != 0)
		return;
	len = *vl;
	for ( mask = vm[0], i = 0; i < len; i++, mask <<= 1 ) {
		if ( i == 64 ) {
			mask	= vm[1];
		}
		if ( mask < 0 ) {
			if ( first_error ) {
				first_error		= 0;
				first_error_start	= start[i];
				first_error_end		= end[i];
				first_error_incr	= incr[i];
			}
			else if ( first_error_start != start[i] ||
			          first_error_end   !=   end[i] ||
			          first_error_incr  !=  incr[i] ) {
				all_same	= 0;
				break;
			}
		}
	}
	if ( all_same ) {
		(void) _fwarn(FWARGSBR, first_error_start,
				first_error_end, first_error_incr, *dim,
				variable, *line, file, *lowerbnd,
				*upperbnd);
		return;
	}
	for ( mask = vm[0], i = 0; i < len; i++, mask <<= 1 ) {
		if ( i == 64 ) {
			mask	= vm[1];
		}
		if ( mask < 0 ) {
			(void) _fwarn(FWARGSBR, start[i], end[i], incr[i],
					*dim, variable, *line, file,
					*lowerbnd, *upperbnd);
		}
	}
}

/*
 *  Error handler for an of out of bounds substring.
 *
 *  Input:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of array which had an out of bounds substring.
 *    size     - Substring size.
 *    start    - Out of bounds substring start.
 *    length   - Out of bounds substring length.
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */
void
_SBOUNDS_ERROR(
		char *file,
		int *line,
		char *variable,
		int *size,
                int *subst,
                int *subln,
                int *count )
{
	int	*retcnt;	/* ptr to static arg count word */
	int	intcnt = 0;	/* local count if no count passed */
	int	endst;
#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 7)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;

	/* if substring length is zero or negative, not incorrect */
	if ( *subln > 0) {
		if ((*retcnt)++ == 0) {

			/* calculate substring end.
			 * subln is calculated by (ln = s2 - s1 + 1)
			 * endst is calculated by (s2 = ln + s1 - 1)
			 */
			endst	= *subln + *subst - 1;
#ifdef KEY /* Bug 7969 */
		if (want_abort()) {
		  (void) _lerror(_LELVL_MSG, FWARGSTR, *subst, endst, variable,
		    *line, file, *size);
		  do_abort();
		}
#endif /* KEY Bug 7969 */
			(void) _fwarn (FWARGSTR, *subst, endst, variable,
			   *line, file, *size);
		}
	}
	return;
}

#if	defined(__mips) || defined(_LITTLE_ENDIAN)
void
__f90_bounds_check(char *procedure_name, _f_int4 line_number, char *array_name, _f_int4 axis_number)
{
	char	*unknown_nm = "name_unknown";
	char	*abort_now = NULL;
	char	*rtn_nm = "__f90_bounds_check";

	/* note mips f90 compiler appends extra characters at
	 * the end of the procedure name.
	 */
 
	/* MIPSpro 7.2 and 7.2.1 documentation assumed that the
	 * routine would abort if this environment variable is
	 * is set.  Otherwise, the message is just a warning.
	 */
	abort_now	= getenv("F90_BOUNDS_CHECK_ABORT");
	if (abort_now) {
		if (*abort_now == 'y' || *abort_now == 'Y') {
			if (array_name) {
				(void) _lerror(_LELVL_MSG, FWARGSVB, 
				   axis_number, array_name, line_number,
				   procedure_name, rtn_nm);
			} else {
				(void) _lerror(_LELVL_MSG, FWARGSVB,
				   axis_number, unknown_nm, line_number,
				   procedure_name, rtn_nm);
			}

			/* cleanup the fortran units before abort */
			_fcleanup();
			abort();
		}
	}
	if (array_name)
		(void) _fwarn(FWARGSVB,axis_number, array_name,
		   line_number, procedure_name, rtn_nm);
	else
		(void) _fwarn(FWARGSVB,axis_number, unknown_nm,
		   line_number, procedure_name, rtn_nm);
	return;
}
#endif		/* end __mips */
