/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 PathScale, Inc.  All Rights Reserved.
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


/* Support swapRM */
#if defined(mips) && !defined(__GNUC__)

#include <ieeefp.h>
extern fp_rnd swapRM(fp_rnd);

#elif defined(TARG_MIPS)

#include <fenv.h>
static inline int swapRM(int rmnew)
{
  // MIPS Rounding mode is stored in last two bits of FCSR register and
  // accessed via assembly instructions cfc1 (load) and ctc1 (store).
  int rmold;
  rmnew &= 0x3;
  asm ( "cfc1 %0,$f31" : "=&r" (rmold) : : "$f31" );
  rmnew |= (rmold & -4);
  asm ( "ctc1 %0,$f31" : : "r" (rmnew) : "$f31" );
  return rmold & 0x3;
}

#elif defined(__GNUC__)

#include <fenv.h>
extern int swapRM(int);

#endif

