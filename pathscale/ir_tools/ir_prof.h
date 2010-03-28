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


typedef long ADDR;

#define C_FIDENT 8
#define COUNTSMAG0    'p'
#define COUNTSMAG1    'M'
#define COUNTSMAG2    'o'
#define COUNTSMAG3    '0'
#define COUNTSMAG4    '~'
#define COUNTSMAG5    '?'
#define COUNTSMAG6    '>'
#define COUNTSMAG7    '&'

#define C_VERSION     1

typedef struct {
  unsigned char c_ident[C_FIDENT];
  int           c_entry;
  short         c_version;
  short         c_dummy1;
} Counts_hdr;

typedef struct {
  ADDR    caller;
  ADDR    callee;
  int     caller_name_idx;
  int     callee_name_idx;
  int     count;
} counts_entry;

struct counts_desc {
  ADDR    caller;
  ADDR    callee;
  struct counts_desc *next;
  int     count;
};



typedef enum {
  ER_FATAL,
  ER_WARNING,
  ER_INFO,
  ER_ERROR,
  ER_VERBOSE,
  ER_MSG,
} error_number;

typedef enum {
  CMP_LESS,
  CMP_SAME,
  CMP_MORE,
} cmp_status;
  

extern void ir_prof_error(int, char *, char *);
