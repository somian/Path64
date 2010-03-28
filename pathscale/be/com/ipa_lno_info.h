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



#ifndef _IPA_LNO_INFO
#define _IPA_LNO_INFO

// Values of sh_info field for section type SHT_MIPS_IPALNO

#define IPA_NULL             0x0
#define IPA_PROCEDURE        0x1
#define IPA_PROJECTED_REGION 0x2
#define IPA_PROJECTED_ARRAY  0x3
#define IPA_TERM_ARRAY       0x4
#define IPA_IVAR             0x5
#define IPA_FORMAL           0x6
#define IPA_GLOBAL           0x7
#define IPA_VALUE	     0x8
#define IPA_EXPR	     0x9
#define IPA_STRINGS          0xa
#define IPA_REVISION 	     0xb 

// Names of these sections

#define IPA_PROCEDURE_NAME          "IPA.procedures"
#define IPA_PROJECTED_REGION_NAME   "IPA.regions"
#define IPA_PROJECTED_ARRAY_NAME    "IPA.projected_arrays"
#define IPA_TERM_ARRAY_NAME         "IPA.terms"
#define IPA_IVAR_NAME               "IPA.ivars"
#define IPA_FORMAL_NAME             "IPA.formals"
#define IPA_GLOBAL_NAME             "IPA.commons"
#define IPA_VALUE_NAME              "IPA.values"
#define IPA_EXPR_NAME               "IPA.exprs"
#define IPA_STRINGS_NAME            "IPA.strings"
#define IPA_REVISION_NAME 	    "IPA.revision"

#define IPA_LNO_SECTION_COUNT 	12

#endif /* IPA_LNO_INFO */

