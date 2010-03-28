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


#include "basic.h"

/*
 * option_seen keeps track of whether an option has been seen,
 * and the order in which options are seen. 
 */

extern void init_option_seen (void);	/* init the option_seen list */
extern void double_max_option_seen (void); /* double the size of the lists */

/* whether option flag was seen or not, includes implicitly seen */
extern boolean option_was_seen (int optflag);
/* whether option flag was implicitly seen */
extern boolean option_was_implicitly_seen (int optflag);

/* set option as seen */
extern void add_option_seen (int optflag);
/* set option as seen, put at beginning of order list */
extern void prepend_option_seen (int optflag);
/* set option as implicitly seen */
extern void add_option_implicitly_seen (int optflag);

/* replace old option seen with new value, but in same place */
extern void replace_option_seen (int old_optflag, int new_optflag);

/* set option as unseen */
extern void set_option_unseen (int optflag);

/*
 * iterator routines:
 */
#define FOREACH_OPTION_SEEN(i)	\
	for (i = first_option_seen(); more_option_seen(); i = next_option_seen())
	
extern int first_option_seen (void);
extern int next_option_seen (void);
extern boolean more_option_seen (void);

/* Warning:  the following two routines only work if inside an iterator */
/* return true if this option was repeated later */
extern boolean current_option_seen_later (int optflag);
/* set current option unseen */
extern void set_current_option_unseen (void);

