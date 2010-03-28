/*
 *  Copyright (C) 2007, 2008. Pathscale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */


#if defined(BUILD_OS_DARWIN)
#include <stdlib.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <malloc.h>
#endif /* defined(BUILD_OS_DARWIN) */

// Use the alternate malloc algorithm indicated by ALG_NUM.
void
__pathscale_malloc_alg (int alg_num)
{
#if ! defined(BUILD_OS_DARWIN) /* Mallopt not supported */
  // Currently there are three alternate algorithms.  Bug 10736
  switch (alg_num){
   case 1:
   default:
       mallopt(M_MMAP_MAX, 2);
       mallopt(M_TRIM_THRESHOLD, 0x10000000);
       break;
   case 2:
       mallopt(M_MMAP_MAX, 2);
       mallopt(M_TRIM_THRESHOLD, 0x40000000);
       break;
   case 3:
       mallopt(M_MMAP_MAX, 0);
       mallopt(M_TRIM_THRESHOLD, -1);
       break;
  }       
#endif /* defined(BUILD_OS_DARWIN) */
}
