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


#ifndef dwarf_DST_dump_INCLUDED
#define dwarf_DST_dump_INCLUDED



#ifdef _KEEP_RCS_ID
static char *dwarf_DST_dump_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "dwarf_DST.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Saves the dump file name for use in the DST_dump() routine!
*/
extern void DST_set_dump_filename(char *file_name);


/* The main dumping routine!
*/
extern void DST_dump(DST_DIR_IDX  incl_dirs,
		     DST_FILE_IDX files,
		     DST_INFO_IDX compile_unit);

/* Alternate entry to dump routine:
 * finds idx values implicitly;
 * passes in the file to dump to, == stdout if f == NULL.
 */
extern void Dump_DST (FILE *f);

#ifdef __cplusplus
}
#endif
#endif /* dwarf_DST_dump_INCLUDED */
