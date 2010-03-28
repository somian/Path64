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


#pragma ident "@(#) libu/util/samefile.c	92.1	07/07/99 13:18:33"

/*    samefile.c     */
#include <sys/types.h>
#include <sys/stat.h>

SAMEFILE (file1, file2)
long *file1;
long *file2;
{
      struct stat st1;
      struct stat st2;

      if (stat((char *)file1, &st1) < 0 || stat((char *)file2, &st2) < 0)
		return(0);
      if (st1.st_ino != st2.st_ino || st1.st_dev != st2.st_dev)
		return(0);
      return(1);
}
