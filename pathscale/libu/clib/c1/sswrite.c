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


#pragma ident "@(#) libu/clib/c1/sswrite.c	92.1	07/01/99 12:16:22"

#include  <sys/param.h>
/*
 *	SSWRITE - write words to secondary data segment
 */
SSWRITE(pds, sds, words )                   
long 	*pds, *sds, *words;
{
	long  n, count;
	char *ermsg = "Error: The number of words to be written by SSWRITE must be a multiple of 512\n";
/*
 *	If the number of words to write is zero, simply return.
 */
	if (*words == 0)
		return(0);

	count = *words/512;
	if (count*512 != *words){
		write(2,ermsg,strlen(ermsg));
		return(-1L);
	}
	n = sswrite((char*)pds, (char*)*sds, count);
	return(n);
 
}
