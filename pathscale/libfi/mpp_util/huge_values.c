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


#pragma ident "@(#) libfi/mpp_util/huge_values.c	92.1	07/13/99 10:45:02"


/**************************************************************************
 *
 * This file contains utility subroutines that are used by the
 * MPP version of the Fortran 90 array intrinsics.
 *
 *************************************************************************/

#include <fortran.h>
#include <fmath.h>


/****************************************************************************
*
*       HUGE_INT returns the largest largest legal value for an integer.
*
****************************************************************************/

int
HUGE_INT()
{
        return (HUGE_INT8_F90);
}

/****************************************************************************
*
*       HUGE_FLT returns the largest largest legal value for an integer.
*
****************************************************************************/

float
HUGE_FLT()
{
        return (HUGE_REAL8_F90);
}
