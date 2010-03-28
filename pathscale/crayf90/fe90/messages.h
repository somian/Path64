/*

  Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.

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



/* USMID:  "\n@(#)5.0_pl/headers/messages.h	5.1	04/29/99 21:22:31\n" */


/* This structure keeps track of previously issued messages.  It is used to */
/* prevent lex from issuing the same msg twice, when reset_lex is called.   */

struct	last_msg_queue_entry	{Uint		msg_num		: 32;
				 Uint		line_num	: 24;
				 Uint		col_num		:  8;
				};

typedef	struct	last_msg_queue_entry	last_msg_queue_type;

static	last_msg_queue_type	last_msg_queue[LAST_MSG_QUEUE_SIZE]	= {
					{0,0,0},
					{0,0,0},
					{0,0,0},
					{0,0,0},
					{0,0,0}};

       char	*command_name;	
       char	 group_code[]	= "cf95";

# if defined(_TARGET_OS_LINUX) || defined(_TARGET_OS_DARWIN)
       char      message_prefix[]       = PSC_NAME_PREFIX "f95";
# elif defined(_TARGET_OS_IRIX)
       char      message_prefix[]       = "f90";
# else
       char      message_prefix[]       = "cf90";
#endif


/* String names of message types.  Must be in step with enum msg_severities   */

static char	*msg_severity_name[]	= {"COMMENT",
					   "NOTE",
					   "CAUTION",
					   "WARNING",
					   "ERROR",
					   "INTERNAL",
					   "VECTOR",
					   "SCALAR",
					   "TABLE",
					   "ANSI",
					   "WARNING",	/*log warning   */
					   "INLINE",
					   "INFO",
					   "TASKING",
					   "LIMIT",
					   "ERROR",	/* log error */
					   "SUMMARY",	/* log summary */
					   "F77_ANSI",	/* F77 ANSI */	  
   					   "OPTIMIZATION",
   					   "STREAM",
					   "UNKNOWN"};

static int	 relative_order;
static int	 save_column_num;
static int	 save_glb_line_num;

       nl_catd	 msg_sys; 


/******************************************************************************\
|*  Function prototypes for functions accessed only from messages.c.          *|
\******************************************************************************/

extern void     output_msg (int, int, msg_severities_type,
                            int, long, long, long, long);


/******************************************************************************\
|*									      *|
|*  Only messages.c and fecif.c need to know about the following variables.   *|
|*									      *|
\******************************************************************************/

extern int	 last_msg_file_rec;
extern char	 cif_tmp_file_name[];


/******************************************************************************\
|*  Function prototype for print_buffered_messages (accessed only from        *|
|*  messages.c and main.c).                                                   *|
\******************************************************************************/

extern void     print_buffered_messages (void);

