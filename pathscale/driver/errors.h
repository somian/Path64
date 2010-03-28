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

#include <stdarg.h>
#include "basic.h"

/* return codes for driver */
typedef int status_codes;
extern int internal_error_occurred;	/* for return/exit */
extern status_codes error_status;	/* for return/exit */

extern char *program_name;		/* name of invoked program */

extern boolean print_warnings;		/* whether to print warning msgs */
extern boolean fullwarn;		/* whether to print all warnings */
extern boolean pass_exit_codes;         /* If true, remember the highest
					 * exit code. */

extern void init_error_list ();

extern void error (char *format, ...)
     __attribute__((format (printf, 1, 2)));

extern void parse_error (const char *name, const char *msg);

extern void warning (char *format, ...)
     __attribute__((format (printf, 1, 2)));

extern void warn_ignored (char *name);

extern void warn_nyi (char *name);

extern void warn_no_longer_needed (char *name);
extern void warn_no_longer_supported (char *name);
extern void warn_no_longer_supported2 (char *name, char *newname);

extern void internal_error (char *format, ...)
     __attribute__((format (printf, 1, 2)));

/* squirrel away error info for bug reporting */
extern void vlog_error (char *format, va_list ap);
extern void log_error (char *format, ...)
     __attribute__((format (printf, 1, 2)));

/* to signal that an error occured but trust previous error messages */
extern void nomsg_error (int);

/* has_errors returns true if were any errors anywhere */
extern boolean has_errors (void);
/* has_current_errors returns true if were any errors in current compile */
extern boolean has_current_errors (void);
/* clear_current_errors resets the error count for a new current compile */
extern void clear_current_errors (void);

