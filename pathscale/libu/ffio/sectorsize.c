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


#pragma ident "@(#) libu/ffio/sectorsize.c	92.1	06/29/99 13:16:47"


#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

_get_sector_size(char *name)
{
int status ;
struct stat check_stat ;
int nbytes ;
char local_name[256] ;
char *last_slash ;

   strcpy( local_name, name ) ;
   last_slash = strrchr( local_name, '/' ) ;
   if( last_slash == NULL )
   {
      strcpy(local_name, "." ) ;
   }
   else
   {
      last_slash[0] = '\0' ;
   }

   status = stat( local_name, &check_stat ) ;
   if( status == -1 )
   {
      return(1) ;
   }

   nbytes = check_stat.st_blksize ;
   if( nbytes == 0 ) nbytes = 4096 ;
   return( nbytes/4096) ;
   
}
