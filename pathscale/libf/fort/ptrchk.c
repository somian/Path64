/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
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



#pragma ident "@(#) libf/fort/ptrchk.c	92.2	06/24/99 10:18:36"
#include <fortran.h>
#include <liberrno.h>
#include <stddef.h>
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
#include <stdlib.h>
#endif

/*
 *  Error handler for an unassociated POINTER reference warning.
 *  This entry is called by the cf90 compiler only.
 *
 *  Input Arguments:
 *    file     - File name in which error occurred.
 *    line     - Line number in file.
 *    variable - Name of pointer that is unassociated.
 *    dv_desc  - determines either "POINTER", "ALLOCATABLE array"
 *                or "ASSUMED SHAPE array".
 *    count    - Static count/flag to indicate if this message was
 *               already given for this statement.
 */

void
_POINTER_ERROR(
	char	*file,		/* Fortran routine containing error	*/
	int	*line,		/* Line number in Fortran routine	*/
	char	*variable,	/* unassociated pointer variable name	*/
	int	*dv_desc,	/* either "POINTER", "ALLOCATABLE array" */
				/* or "ASSUMED SHAPE array".		*/
	int	*count)		/* Count/flag for number of messages	*/
{
	int	*retcnt;	/* ptr to static arg count word		*/
	int	intcnt = 0;	/* local count if no count passed	*/
#ifdef	_UNICOS
	/* Use local variable if count argument not passed. */
	if (_numargs() < 5)
		retcnt	= &intcnt;
	else
#endif
		retcnt	= count;
	if ((*retcnt)++ == 0) {
#ifdef KEY /* Bug 10378 */
                int msg = 0;
		if (*dv_desc == 1) {
                	msg = FWASOCPT;
                } else if (*dv_desc == 2) {
			msg = FWALOCAR;
                } else if (*dv_desc == 3) {
			msg = FWASASAR;
		}
		char *abort_now = getenv("F90_BOUNDS_CHECK_ABORT");
		if (abort_now && (*abort_now == 'y' || *abort_now == 'Y')) {
		  (void) _lerror(_LELVL_MSG, msg, variable, *line, file);
		  _fcleanup();
		  abort();
		}
		(void) _fwarn(msg, variable, *line, file);
#else /* KEY Bug 10378 */
		if (*dv_desc == 1) {
                	(void) _fwarn(FWASOCPT, variable, *line, file);
                } else if (*dv_desc == 2) {
			(void) _fwarn(FWALOCAR, variable, *line, file);
                } else if (*dv_desc == 3) {
			(void) _fwarn(FWASASAR, variable, *line, file);
		}
#endif /* KEY Bug 10378 */
	}
	return;
}
