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


#pragma ident "@(#) libu/aqio/aqread.c	92.1	07/01/99 13:50:36"
 
#include <liberrno.h>
#include <fortran.h>
#include <sys/types.h>
#include <sys/iosw.h>
#include <sys/listio.h>
 
/*
 *	AQREAD - queue an asynchronous read request
 */

void
AQREAD(aqp,cpuadd,blknum,blocks,reqid,queue,status)
long *aqp, *blknum, *blocks, *reqid, *queue, *status;
void *cpuadd;
{
	long incs = 0;
	long mstride = 0;
	long dstride = 0;

	if (_issddptr(cpuadd)) {
		*status  = -FESHRSUP;
		return;
	}
	_aqenq(LO_READ,aqp,cpuadd,&mstride,blknum,blocks,&dstride,&incs,reqid,queue,status);
	return;
}

/*
 *	AQREADC - queue an asynchronous compound read request
 */

void
AQREADC(aqp,cpuadd,mstride,blknum,blocks,dstride,incs,reqid,queue,status)
long *aqp, *mstride, *blknum, *blocks, *dstride, *incs,
		*reqid, *queue, *status;
void *cpuadd;
{
	if (_issddptr(cpuadd)) {
		*status  = -FESHRSUP;
		return;
	}
	_aqenq(LO_READ,aqp,cpuadd,mstride,blknum,blocks,dstride,incs,reqid,queue,status);
	return;
}
