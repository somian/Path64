/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* ====================================================================
 * ====================================================================
 *
 * Module: sgi_cmd_line.h
 * $Revision: 1.10 $
 * $Date: 05/08/26 23:50:11-07:00 $
 * $Author: fchow@fluorspar.internal.keyresearch.com $ 
 * $Source: crayf90/sgi/SCCS/s.sgi_cmd_line.h $
 *
 * Revision history:
 *  09-26-95 - Original Version
 *
 * Description:
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef SGI_CMD_LINE_INCLUDED
#define SGI_CMD_LINE_INCLUDED

#ifdef _KEEP_RCS_ID
static char *rcs_id = "$Source: crayf90/sgi/SCCS/s.sgi_cmd_line.h $ $Revision: 1.10 $";
#endif /* _KEEP_RCS_ID */

extern char *FE_command_line;
extern char **save_argv;
extern INT8 Debug_Level;
extern INT save_argc;

extern BOOL IO_Comments;
extern BOOL Use_Three_Call;
extern BOOL FE_Full_Split_Set;
extern BOOL FE_Full_Split;
extern BOOL FE_Endloop_Marker;
extern BOOL Full_arrayexp;
extern mUINT16  FE_align ;

extern char *rii_file_name;
extern BOOL enable_dsm_recompile;
extern BOOL enable_dsm_processing;

extern char *FE_gdar_filename;
#ifdef KEY
extern char *F2C_ABI_filename;
#endif

extern BOOL  global_chunk_pragma_set;   /* From the -chunk= command line option */
extern INT32 global_chunk_pragma_value;

extern INT32 global_schedtype_pragma_val; /* From the -mp_schedtype command line option */
extern BOOL global_schedtype_pragma_set;

extern BOOL process_cri_mp_pragmas;

extern BOOL FE_Call_Never_Return;

extern void add_cray_args ( const char * );
extern void Cray_Woff ( char * );

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void  sgi_cmd_line(int *argc, char **argv[]);

#ifdef KEY /* Bug 4719 */
/* Name of file specified with "-o filename", used when -E is in effect */
extern  char *preprocessor_output_file;
#endif /* KEY Bug 4719 */
#ifdef KEY /* Bug 4260 */
/* Value of -byteswapio or -convert option */
extern int io_byteswap;
#endif /* KEY Bug 4260 */
#ifdef KEY /* Bug 3507 */
extern BOOL option_underscoring;
extern BOOL option_second_underscore;
#endif /* KEY Bug 3507 */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* SGI_CMD_LINE_INCLUDED */

