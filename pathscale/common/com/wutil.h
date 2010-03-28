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


#ifndef wutil_INCLUDED
#define wutil_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "wintrinsic.h"
#include "wio.h"

/* ====================================================================
 * ====================================================================
 *
 * Module: wutil.h
 *
 * Description:
 *
 * This module contains the external interfaces (data structures and
 * prototypes) for routines which provide utility services related to
 * Whirl intrinsics.
 *
 * ====================================================================
 * ====================================================================
 */

const char *get_intrinsic_name(INTRINSIC opcode);
const char *get_iostatement_name(IOSTATEMENT opcode);
const char *get_ioitem_name(IOITEM opcode);
const char *get_iolibrary_name(IOLIB opcode);

#define INTRINSIC_name(op) get_intrinsic_name(op)
#define IOSTATEMENT_name(op) get_iostatement_name(op)
#define IOITEM_name(op) get_ioitem_name(op)

#ifdef __cplusplus
}
#endif
#endif /* wutil_INCLUDED */
