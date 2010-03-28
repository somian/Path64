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


/* USMID @(#) libu/ffio/sys_parse.h	92.0	10/08/98 14:57:41 */

/*  SYSTEM LAYER ASSIGN PARSING DEFINITION */

#ifdef	__mips
struct LAYER_OPTS __sys_opts[] = {
CLASS_SYSTEM, 0, 0, SYS_flags_AIO_CB, 0               , 0, 0, 0, "noaiocb",
CLASS_SYSTEM, 0, 0, SYS_flags_AIO_CB, SYS_flags_AIO_CB, 0, 0, 0, "aiocb"  ,
};
#define NUM_SYSTEM_OPTS     2
#else
#define NUM_SYSTEM_OPTS     0
#endif

#define NUM_SYSTEM_NUMERICS 0
#define NUM_SYSTEM_ALIAS    0

struct LAYER_DATA _system_data =
    {
         CLASS_SYSTEM,
         NO_TYPE,
         "system",
#ifdef	__mips
         "noaiocb",
#else
         "",
#endif
         0,
         0,
         NUM_SYSTEM_OPTS,
         1 ,
         NUM_SYSTEM_NUMERICS,
         NUM_SYSTEM_ALIAS,
#ifdef	__mips
         __sys_opts,
#else
         NULL,
#endif
         NULL,
         NULL,
         NULL
    };
