/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


static char USMID[] = "@(#) libcif/ciferrstr.c	30.2	07/26/96 07:19:13";


/* -------------------------------------------------------------------------
 * Cif_Errstring returns a pointer to a character string that describes the
 * error condition associated with a CIF status.
 * -------------------------------------------------------------------------
 */

#define CIF_VERSION 3

#include <string.h>
#ifdef _ABSOFT
#include "cif.h"
#else
#include <cif.h>
#endif

#include <errno.h>

static char *strings[] = {
	"",
	"The file is not a CIF format file",
	"The maximum number of CIF files are already open",
	"The CIF descriptor is not attached to an open CIF file",
	"No more memory can be acquired",
	"The end of a CIF file has been encountered",
	"An internal error in the CIF interface routines has occurred",
	"The requested CIF function cannot be performed",
	"The CIF file contains invalid data",
	"",
	"The CIF version specified to Cif_Open is greater than allowed",
	"The CIF version encountered in the file is greater than allowed",
	"The CIF version specified to Cif_Open is less than allowed",
	"libcif.a does not match the cif.h file compiled into this application",
};
static char *unknown = "Unknown CIF status code";

char *Cif_Errstring
#ifdef __STDC___
(int status)
#else
(status)
int status;			/* cif error status code */
#endif
{
	if (status >= 0 || status < CIF_MAXERROR)
		return (unknown);
	else if (status == CIF_SYSERR) {
		int err_num = errno;
		int ok;
		char *err_str;
		
		errno = 0;
		err_str = strerror(err_num);
		ok = errno == 0;
		errno = err_num;
	
		if (ok)
			return (err_str);
		else
			return (unknown);
	}
	else
		return (strings[-status]);
}
