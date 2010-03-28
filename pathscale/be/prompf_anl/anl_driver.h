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


#ifndef anl_driver_INCLUDED
#define anl_driver_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
/* ==============================================================
 * ==============================================================
 *
 * Module: anl_driver.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 *   Defines utilities exported by prompf_anl.so.  The map returned
 *   by Anl_Static_Analysis() will be used later by whirl2[cf] and
 *   by LNO to emit information about transformed constructs.
 *
 * ==============================================================
 * ==============================================================
 */

#if defined(BUILD_OS_DARWIN)
/* Linux expects to declare these as weak symbols but never define them.
 * Mach-O linker doesn't permit that. */
#define Anl_Process_Command_Line(a, b, c, d) assert(!"Anl_Process_Command_Line")
#define Anl_Needs_Whirl2c() 0
#define Anl_Needs_Whirl2f() 0
#define Anl_Init() assert(!"Anl_Init")
#define	Anl_Init_Map(a) 0
#define Anl_Static_Analysis(a, b) assert(!"Anl_Static_Analysis")
#define Get_Next_Construct_Id() 0L
#define New_Construct_Id() 0L
#define Anl_Fini() assert(!"Anl_Fini")
#define Anl_Cleanup() assert(!"Anl_Cleanup")
#define Anl_File_Path() 0
#else /* defined(BUILD_OS_DARWIN) */
void        Anl_Process_Command_Line (INT phase_argc, char *phase_argv[],
				      INT argc, char *argv[]);
BOOL        Anl_Needs_Whirl2c(void);
BOOL        Anl_Needs_Whirl2f(void);
void        Anl_Init(void);
WN_MAP	    Anl_Init_Map(MEM_POOL *id_map_pool); 
void        Anl_Static_Analysis(WN *pu, WN_MAP id_map);
INT64       Get_Next_Construct_Id(void);
INT64       New_Construct_Id(void);
const char *Anl_File_Path(void);
void        Anl_Fini(void);
void        Anl_Cleanup(void);
#endif /* defined(BUILD_OS_DARWIN) */

#ifdef __cplusplus
}
#endif
#endif /* anl_driver_INCLUDED */
