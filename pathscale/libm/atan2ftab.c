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



/* ====================================================================
 * ====================================================================
 *
 * Module: atan2ftab.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  09-Jun-93 - Original Version
 *
 * Description:	error tables for atan2f function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source$ $Revision$";

#include "libm.h"

const fu	_atan2fres0[] =
{
{0x00000000},
{0x80000000},
{0x40490fdb},
{0xc0490fdb},
};

const fu	_atan2fres1[] =
{
{0x3fc90fdb},
{0xbfc90fdb},
{0x3fc90fdb},
{0xbfc90fdb},
};

const fu	_atan2fres2[] =
{
{0x00000000},
{0x80000000},
{0x40490fdb},
{0xc0490fdb},
};

const fu	_atan2fres4[] =
{
{0x00000000},
{0x00000000},
{0x3fc90fdb},
{0xbfc90fdb},
{0x00000000},
{0x00000000},
{0x3fc90fdb},
{0xbfc90fdb},
{0x00000000},
{0x80000000},
{0x3f490fdb},
{0xbf490fdb},
{0x40490fdb},
{0xc0490fdb},
{0x4016cbe4},
{0xc016cbe4},
};

