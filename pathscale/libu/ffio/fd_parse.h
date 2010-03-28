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


/* USMID @(#) libu/ffio/fd_parse.h	92.0	10/08/98 14:57:41 */

/*  FD LAYER ASSIGN PARSING DEFINITION */

#define NUM_FD_ALIAS    0
#define NUM_FD_OPTS     0


struct LAYER_NUMERICS _fd_numerics[]  = {
   { 1, 0, 'n' , 0 , 0xfffff , 0,  "file_descriptor" } , /*  number of blocks for buffer */
                     } ;
#define NUM_FD_NUMERICS (sizeof(_fd_numerics)/sizeof(struct LAYER_NUMERICS))

struct LAYER_DATA _fd_data =
    {
         CLASS_FD ,
         FLD_EXT_TYPE,
         "fd",
         "" ,
         0,
         0,
         NUM_FD_OPTS,
         1 ,
         NUM_FD_NUMERICS,
         NUM_FD_ALIAS ,
         NULL,
         _fd_numerics ,
         NULL,
         NULL
    } ;
struct LAYER_DATA _stdin_data =
    {
         CLASS_FD ,
         FLD_EXT_TYPE,
         "stdin",
         ":0" ,
         0,
         0,
         NUM_FD_OPTS,
         1 ,
         1,
         NUM_FD_ALIAS ,
         NULL,
         _fd_numerics ,
         NULL,
         NULL
     };
struct LAYER_DATA _stdout_data =
    {
         CLASS_FD ,
         FLD_EXT_TYPE,
         "stdout",
         ":1" ,
         0,
         0,
         NUM_FD_OPTS,
         1 ,
         1,
         NUM_FD_ALIAS ,
         NULL,
         _fd_numerics ,
         NULL,
         NULL
     };
struct LAYER_DATA _stderr_data =
    {
         CLASS_FD ,
         FLD_EXT_TYPE,
         "stderr",
         ":2" ,
         0,
         0,
         NUM_FD_OPTS,
         1 ,
         1,
         NUM_FD_ALIAS ,
         NULL,
         _fd_numerics ,
         NULL,
         NULL
     };
