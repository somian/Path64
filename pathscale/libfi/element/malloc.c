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


#pragma ident "@(#) libfi/element/malloc.c	92.2     08/19/99 11:34:13"

#include <fortran.h>
#include <cray/portdefs.h>
#include <malloc.h>

extern _f_int4 _MALLOC_I4_I4(_f_int4);
extern _f_int4 _MALLOC_I4_I8(_f_int8);
extern _f_int8 _MALLOC_I8_I4(_f_int4);
extern _f_int8 _MALLOC_I8_I8(_f_int8);

_f_int4 _MALLOC_I4_I4(_f_int4 size)
{
  void *addr;

  if(size == 0)
    return 0;
  else
    {
      addr = malloc(size);
      return ((_f_int4) addr);
    }
}

_f_int4 _MALLOC_I4_I8(_f_int8 size)
{
  void *addr;

  if(size == 0)
    return 0;
  else
    {
      addr = malloc(size);
      return ((_f_int4) addr);
    }
}

_f_int8 _MALLOC_I8_I4(_f_int4 size)
{
  void *addr;

  if(size == 0)
    return 0;
  else
    {
      addr = malloc(size);
      return ((_f_int8) addr);
    }
}

_f_int8 _MALLOC_I8_I8(_f_int8 size)
{
  void *addr;

  if(size == 0)
    return 0;
  else
    {
      addr = malloc(size);
      return ((_f_int8) addr);
    }
}
