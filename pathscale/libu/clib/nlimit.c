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

#pragma ident "@(#) libu/clib/nlimit.c	92.1	07/01/99 13:42:20"

#include <errno.h>
#include <sys/resource.h>

/*
 * NLIMIT is the fortran library interface to nlimit(3).  It expects two
 * parameters: an id (pid, sid, or uid) and a fortran array.  The code 
 * translates the array into the resclim structure and calls nlimit(3).
 * Upon successful completion the structure is translated back into the
 * array and returns a 0 return code.  If the nlimit(3) call fails, errno
 * is returned.
 */

long
NLIMIT(tid, res_array, errstat)
long *tid;
long res_array[10];
long *errstat;
{
	struct resclim rs;

	rs.resc_resource = res_array[0];
	rs.resc_category = res_array[1];
	rs.resc_type     = res_array[2];
	rs.resc_action   = res_array[3];
	rs.resc_used     = res_array[4];
	rs.resc_value[L_T_ABSOLUTE] = res_array[5];
	rs.resc_value[L_T_HARD] = res_array[6];
	rs.resc_value[L_T_SOFT] = res_array[7];

	if (nlimit(*tid, &rs) == -1) {
		*errstat = errno;
		return(-1);
	}

	res_array[0] = rs.resc_resource;
	res_array[1] = rs.resc_category;
	res_array[2] = rs.resc_type;
	res_array[3] = rs.resc_action;
	res_array[4] = rs.resc_used;
	res_array[5] = rs.resc_value[L_T_ABSOLUTE];
	res_array[6] = rs.resc_value[L_T_HARD];
	res_array[7] = rs.resc_value[L_T_SOFT];
	return(0);
}
