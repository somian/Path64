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


/* USMID @(#) libfi/mathlb/ieee_class_values.h	92.0	10/08/98 14:37:14 */

#ifndef IEEE_CLASS_VALUES_H
#define IEEE_CLASS_VALUES_H

/* classify values to be used from c but to match Fortran values. */

#ifndef IEEE_CLASS_SIGNALING_NAN
#define IEEE_CLASS_SIGNALING_NAN	1
#endif

#ifndef IEEE_CLASS_QUIET_NAN
#define IEEE_CLASS_QUIET_NAN		2
#endif

#ifndef IEEE_CLASS_NEG_INFINITY
#define IEEE_CLASS_NEG_INFINITY		3
#endif

#ifndef IEEE_CLASS_NEG_NORM_NONZERO
#define IEEE_CLASS_NEG_NORM_NONZERO	4
#endif

#ifndef IEEE_CLASS_NEG_DENORM
#define IEEE_CLASS_NEG_DENORM		5
#endif

#ifndef IEEE_CLASS_NEG_ZERO
#define IEEE_CLASS_NEG_ZERO		6
#endif

#ifndef IEEE_CLASS_POS_ZERO
#define IEEE_CLASS_POS_ZERO		7
#endif

#ifndef IEEE_CLASS_POS_DENORM
#define IEEE_CLASS_POS_DENORM		8
#endif

#ifndef IEEE_CLASS_POS_NORM_NONZERO
#define IEEE_CLASS_POS_NORM_NONZERO	9
#endif

#ifndef IEEE_CLASS_POS_INFINITY
#define IEEE_CLASS_POS_INFINITY		10
#endif

#endif	/* !IEEE_CLASS_VALUES_H */
