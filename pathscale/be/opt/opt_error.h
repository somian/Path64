/* ====================================================================
 * ====================================================================
 *
 * Module: opt_error.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  30-AUG-94 - Original Version
 *
 * ====================================================================
 *
 * Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
 *
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
 *
 * ====================================================================
 *
 * Description:
 *             OPT_ERROR(type, errno) 
 *             OPT_ERROR_FMT(type, errno, errmsg_list) 
 *             OPT_ASSERT(cond, message)
 *
 * ====================================================================
 * ====================================================================
 */


#ifndef opt_error_INCLUDED
#define opt_error_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif


#ifdef _KEEP_RCS_ID
static char *opt_error_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */


/*************************************************************************
 *
 *  ER_FATAL:    Fatal error.  Compilation stops.
 *  ER_ERROR:    Optimizer internal error.   Compilation may proceed.
 *  ER_INFO:     Information such as compile-phase, compilation-time, memory-usage, etc.
 *  ER_VERBOSE:  Information such as which PU is being compiled, ... 
 *
 *  ASSERT is ER_FATAL. 
 *
 *   
 *************************************************************************/

#define ER_FATAL        1
#define ER_ERROR        2
#define ER_INFO         3
#define ER_VERBOSE      4


/*
 *  Error number for the optimizer
 */
typedef enum {
  ERN_IGNORED,
  ERN_INTERNAL,
  ERN_OUT_OF_MEM,
  ERN_BAD_OPTION,
      MAX_ERN_NUMBER
  } ERROR_NUMBER;


/* Macros */


/*
 *   Four ways of reporting error:
 *    1.  use error number 
 *    2.  use format-string and a variable number of optional arguments
 *    3.  to assert with one fixed message, use OPT_ASSERT
 *    4.  to assert with more arguments, use OPT_ASSERT_FMT.
 */
#define OPT_ERROR(type, errno) \
                  (opt_Abort_File = __FILE__, opt_Abort_Loc = __LINE__, \
		   OPT_Error(type, errno))
  
#define OPT_ERROR_FMT(type, errmsg_list) \
                      (opt_Abort_File = __FILE__, opt_Abort_Loc = __LINE__,\
                       OPT_Error_Fmt (type, errmsg_list))
  
#define OPT_ASSERT(cond, message) \
                   if (cond) (OPT_ERROR_FMT(ER_FATAL, (message)))

#define OPT_ASSERT_FMT(cond, errmsg_list)  \
                       if (cond) (OPT_ERROR_FMT(ER_FATAL, errmsg_list))


/* Global Variables */

extern char *opt_Abort_File;		/* name of the file that asserted */
extern INT   opt_Abort_Loc;		/* line number that asserted */

/* Error routine */
extern void OPT_Error(INT type, INT errnum, ...);

extern void OPT_Error_Fmt(INT type, const char *fmt_str, ...);

extern void Opt_Catch_Signals (void);

#ifdef __cplusplus
}
#endif

#endif /* opt_error_INCLUDED */
