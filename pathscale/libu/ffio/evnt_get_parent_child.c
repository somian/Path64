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


#pragma ident "@(#) libu/ffio/evnt_get_parent_child.c	92.1	06/29/99 13:16:47"


#include <ffio.h>
#include "evntio.h"

/*
 * _evnt_get_parent_child
 *
 * Pick up names of layer above and below 
 *
 * Input:
 *	fio		- file descriptor
 *	parent		- pointer to name of parent layer (above)
 *	child		- pointer to name of child layer (below)
 *
 * Output:
 *
 */
void
_evnt_get_parent_child(struct fdinfo *fio, char  **parent, char **child)
{
	static char PROGRAM[] = "program";

	char   *LAYER_NAMES[NUM_CLASSES] =
	{
		"system ", "syscall ", "null ", "system ", "cos ", "bmx ", "f ",
		"v ", "text ", "x ", "cdc ", "sds ", "mr ", "trace ",
		"user ", "site ", "error ", "fd ", "blx ",
		"cache ",
		"er90 ",
		"bufa ",
		"cachea ",
		"event ",
		"lock ",
		"global ",
		"f77 ",
		"resv_27 ",
		"resv_28 ",
		"resv_29 ",
		"user_0",
		"user_1",
		"user_2 ",
		"user_3 ",
		"user_4 ",
		"user_5 ",
		"user_6 ",
		"user_7 ",
		"user_8 ",
		"user_9 "
	};

	if (parent) {
		if (fio->parptr == 0)
			*parent = PROGRAM;
		else
			*parent = LAYER_NAMES[fio->parptr->class];
	}
	if (child)
		*child = LAYER_NAMES[fio->fioptr->class];
}
