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
#include <sys/types.h>
#include "defs.h"
#include "srcpos.h"
#include "glob.h"	/* for Tlog_File */
#include "errors.h"	/* for FmtAssert */

static const char* dummy_word="(null)";

/* see interface in tlog.h */

extern void Generate_Tlog(
  const char*   phase_name,
  const char*   trans_name,
  SRCPOS        srcpos,
  const char*   keyword,
  const char*   input_string,
  const char*   output_string,
  const char*   aux_info_string
)
{
  if (Tlog_File==NULL)
    return;

  FmtAssert(phase_name!=NULL,("Null phase name !!"));
  FmtAssert(trans_name!=NULL,("Null transformation name !!"));

  if (keyword[0]=='\0')
    keyword=dummy_word;

  fprintf(Tlog_File, "\n%s %s %" SCNd64 " %s\n",
    phase_name, trans_name, srcpos, keyword);
  fprintf(Tlog_File, "{ %s }\n", input_string);
  fprintf(Tlog_File, "{ %s }\n", output_string);
  fprintf(Tlog_File, "{ %s }\n", aux_info_string);
}


