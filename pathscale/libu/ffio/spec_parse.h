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


/* USMID @(#) libu/ffio/spec_parse.h	92.0	10/08/98 14:57:41 */


#ifndef _FF_SPEC_PARSE
#define _FF_SPEC_PARSE
#include <ffio.h>
#include "layer_def.h"

#if defined(_CRAY) || defined(_MIPSEB)
typedef int64 numerictype;
#else
typedef long numerictype;
#endif

extern int _get_next_token(const char **str_inp, char *token, char *separators,
	int strip_spaces, int lower_case, int maxlen, int errmode);
extern int _set_layer_options(char *layer_name_inp, char *options,
	union spec_u *specs, int spec_level, int limit, int *num_added,
	int warnmode, int errmode);
extern char *_g_alias(char *aliasptr, char *path);
extern char *_g_fchar(char *fcharptr, char *path);
extern int _g_fchar_fd(char *entry);
extern char *_g_rname(char *entry);
extern char *_g_fchar_opt(char *entry, char letter);
extern char *_g_fchar_opts(char *entry);
extern char *_bld_fchar(char *fcharptr, char *filename,int fd, char *optstring);
extern char *_bld_alias(char *aliasptr, char *alias, char *filename);
extern long _hex2bin(char *str);
extern void _bin2hex(char *str, unsigned long num);
extern int _cpyname(char *target, char *src);
extern int _parse_forstr(union spec_u *specs, const char *str, int limit, int warnmode, int errmode);
extern int _class_vf_check(union spec_u *specs, struct LAYER_DATA *table,
    int num, int warnmode, int errmode);
extern int _class_vms_check(union spec_u *specs, struct LAYER_DATA *table,
    int num, int warnmode, int errmode);

#define SET_VALID_BIT(var,val) var.valid = val
#define GET_VALID_BIT(var)     var.valid
#endif
