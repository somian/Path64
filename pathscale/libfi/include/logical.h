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


/* USMID @(#) libfi/include/logical.h	92.0	10/08/98 14:37:14 */
#if defined(_F_LOG4) && defined(_F_LOG8)
#include <limits.h>
#endif

#define TRUE 1
#define FALSE 0
#define BITS_PER_BYTE       (BITS_PER_WORD / BYTES_PER_WORD)

#if	defined(_F_LOG2) && defined(__mips)
#define LTOB(i,a) \
	((i == 8*CHAR_BIT) ? (_lvtob((*(_f_log8 *) (a)))) : \
	((i == 4*CHAR_BIT) ? (_lvtob((*(_f_log4 *) (a)))) : \
	((i == 2*CHAR_BIT) ? (_lvtob((*(_f_log2 *) (a)))) : \
	((i == 1*CHAR_BIT) ? (_lvtob((*(_f_log1 *) (a)))) : \
	(_lvtob((*(_f_log *) (a))))))))
#elif defined(_F_LOG4) && defined(_F_LOG8)
#define LTOB(i,a) \
  ((i == 4*CHAR_BIT) ? (_lvtob((*(_f_log4 *) (a)))) : (_lvtob((*(_f_log8 *) (a)))))
#else
#define LTOB(i,a) (_lvtob((*(_f_log *) (a))))
#endif

