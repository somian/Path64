/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


#include <stdio.h>
#include <stdarg.h>
#include "arith.h"
#include "arith.internal.h"

void PRINTMSG(int pseudo_line_num, int msg_number,
              enum message_types msg_severity, int column_num, ...)
{
   va_list arg_ptr;

   va_start(arg_ptr, column_num);
   if(msg_severity==Internal || msg_severity==Error)
     fprintf(stderr, "ARITH FATAL INTERNAL ERROR #%d:  ", msg_number);
   else
     fprintf(stderr, "ARITH WARNING ERROR #%d:  ", msg_number);
   vfprintf(stderr, "file %s, line#%d\n", arg_ptr);
   va_end(arg_ptr);
   if(msg_severity==Internal || msg_severity==Error) exit(1);
}

static char rcsid [] = "$Id$";
