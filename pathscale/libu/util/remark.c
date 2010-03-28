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


#pragma ident "@(#) libu/util/remark.c	92.1	07/07/99 13:18:33"

#include "fortran.h"

#ifdef _CRAY2
#define MAX_RMK_LEN 152 /* Maximum message length for REMARK */
#define MAX_RMK2_LEN 152 /* Maximum message length for REMARK2 */
#else
#define MAX_RMK_LEN 71 /* Maximum message length for REMARK */
#define MAX_RMK2_LEN 79 /* Maximum message length for REMARK2 */
#endif

static void __remark();
/*
 * REMARK and REMARK2 are Fortran-callable routines which print a 
 * message to stderr.
 */
void
REMARK(s)
_fcd s;		/* Message to be printed as a Fortran character pointer */
{
  
#ifdef _ADDR64
	if (_numargs() == 1) {
		char *saddr;
		saddr = *(char **)&s;
		__remark(_cptofcd(saddr,strlen(saddr)), MAX_RMK_LEN);
	}
	else
#endif
	__remark(s, MAX_RMK_LEN);
}
void
REMARK2(s)
_fcd s;		/* Message to be printed as a Fortran character pointer */
{
#ifdef _ADDR64
	if (_numargs() == 1) {
		char *saddr;
		saddr = *(char **)&s;
		__remark(_cptofcd(saddr,strlen(saddr)), MAX_RMK2_LEN);
	}
	else
#endif
	__remark(s, MAX_RMK2_LEN);
}

static void
__remark(s, maxlen)
_fcd s;		/* Message to be printed as a Fortran character pointer */
int maxlen;	/* Maximum length of message */
{
	char	msg[MAX_RMK2_LEN+1];
	int	len;
  
	len = _fcdlen(s);
	if (len == 0)
		len = strlen(_fcdtocp(s));	/* assume NULL-terminated */
	if (len > maxlen)
		len = maxlen;
  
	/* Copy the message, adding a trailing '\n' */
	strncpy(msg,_fcdtocp(s),len);
	msg[len] = '\n';
	
	write(2,msg,len+1);	/* write message to stderr */
}

