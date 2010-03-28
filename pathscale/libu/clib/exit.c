/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

#pragma ident "@(#) libu/clib/exit.c	92.1	07/01/99 13:42:20"

#include <fortran.h>
#include <stddef.h>
#include <stdlib.h>
  
/*
 *	EXIT	Provide user-callable exit from a program.
 *
 *	CALL EXIT([istat])
 *
 *		istat	Integer exit status.  This is an optional argument
 *			on UNICOS system.  The exit status is zero if this
 *			argument is not specified.
 */

#ifdef	_UNICOS
void
EXIT(status)
_f_int	*status;
{
	exit((_numargs() > 0) ? *status : 0);
}

#else

void
exit_(status)
_f_int	*status;
{
	/*
	 * CF90 on non-UNICOS systems will *sometimes* pass NULL if the user 
	 * called exit with no arguments.  Note that EXIT() might be called
	 * by a subroutine to which EXIT was passed as an actual argument,
	 * in which case CF90 would not add a NULL argument to the call 
 	 * to EXIT().
	 */ 
	exit((status != NULL) ? *status : 0);
}
#endif
