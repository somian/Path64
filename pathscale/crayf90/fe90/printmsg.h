/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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

#ifdef KEY /* Bug 6121 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Note: update 'msg_severity_names' in messages.h if this enum is changed    */
/* Note: all message levels are output to CIF, only Comment .. Internal,      */
/*	 Log_Error, Log_Warning and Ansi are output to stderr		      */

enum	msg_severities	       {Comment,	Note,		Caution,
				Warning,	Error,		Internal,
				Vector,		Scalar,		Table,
				Ansi,		Log_Warning,	Inline,
				Info,		Tasking,	Limit,
				Log_Error,	Log_Summary,	F77_Ansi,
				Optimization,	Stream,		Unknown_Error };
typedef enum	msg_severities			msg_severities_type;

extern void PRINTMSG(int, int, msg_severities_type, int, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* KEY Bug 6121 */
