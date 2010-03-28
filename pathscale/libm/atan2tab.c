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
 * Module: atan2tab.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  09-Jun-93 - Original Version
 *
 * Description:	error tables for atan2 function
 *
 * ====================================================================
 * ====================================================================
 */

static char *rcs_id = "$Source$ $Revision$";

#include "libm.h"

const du	_atan2res0[] =
{
{D(0x00000000, 0x00000000)},
{D(0x80000000, 0x00000000)},
{D(0x400921fb, 0x54442d18)},
{D(0xc00921fb, 0x54442d18)},
};

const du	_atan2res1[] =
{
{D(0x3ff921fb, 0x54442d18)},
{D(0xbff921fb, 0x54442d18)},
{D(0x3ff921fb, 0x54442d18)},
{D(0xbff921fb, 0x54442d18)},
};

const du	_atan2res2[] =
{
{D(0x00000000, 0x00000000)},
{D(0x80000000, 0x00000000)},
{D(0x400921fb, 0x54442d18)},
{D(0xc00921fb, 0x54442d18)},
};

const du	_atan2res4[] =
{
{D(0x00000000, 0x00000000)},
{D(0x00000000, 0x00000000)},
{D(0x3ff921fb, 0x54442d18)},
{D(0xbff921fb, 0x54442d18)},
{D(0x00000000, 0x00000000)},
{D(0x00000000, 0x00000000)},
{D(0x3ff921fb, 0x54442d18)},
{D(0xbff921fb, 0x54442d18)},
{D(0x00000000, 0x00000000)},
{D(0x80000000, 0x00000000)},
{D(0x3fe921fb, 0x54442d18)},
{D(0xbfe921fb, 0x54442d18)},
{D(0x400921fb, 0x54442d18)},
{D(0xc00921fb, 0x54442d18)},
{D(0x4002d97c, 0x7f3321d2)},
{D(0xc002d97c, 0x7f3321d2)},
};

