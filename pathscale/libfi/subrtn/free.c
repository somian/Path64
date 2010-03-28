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


#pragma ident "@(#) libfi/subrtn/free.c	92.2     08/19/99 12:03:07"

#include <fortran.h>
#include <cray/portdefs.h>
#include <malloc.h>

extern void _FREE_I4(_f_int4);
extern void _FREE_I8(_f_int8);

void _FREE_I4(_f_int4 addr)
{
  if(addr == 0)
    return;
  else
    {
      free((void *) addr);
    }
}

void _FREE_I8(_f_int8 addr)
{
  if(addr == 0)
    return;
  else
    {
      free((void *) addr);
    }
}
