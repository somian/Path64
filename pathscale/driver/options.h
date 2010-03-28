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
#include "lang_defs.h"

/*
 * Routines for handling the list of predefined options,
 * plus any user-defined options.
 */

extern void init_options (void);	/* init the options list */

extern char *get_option_name (int flag);	/* return name */
extern char *get_option_help (int flag);	/* return help msg */

/* whether option should have a blank space in it, e.g. -o foo */
extern boolean option_has_blank (int flag);

/* whether the option is valid for the language */
extern boolean option_matches_language (int flag, languages_t l);
/* whether the option is valid for the phase */
extern boolean option_matches_phase (int flag, phases_t p);
/* whether the option is internal (based on language mask) */
extern boolean is_internal_option (int flag);
extern void set_internal_option (int flag);

/* add phase to list of valid phases for option */
extern void add_phase_for_option(int flag, phases_t p);
/* remove phase from list of valid phases for option */
extern void remove_phase_for_option(int flag, phases_t p);
/* set language for option */
extern void set_language_for_option (int flag, languages_t l);

/* add new user-defined option */
extern int add_new_option (char *arg);

/*
 * For options with arbitrary argument, we have the option prefix in 
 * our predefined table/list of options.
 * Now we want to create a new derived entry for the exact option string,
 * which will point back to the prefix entry.  
 */
extern int add_derived_option (int parent, char *arg);
extern boolean is_derived_option (int flag);	/* is option derived? */
extern int get_derived_parent (int flag);	/* return base parent */

/*
 * iterator routines 
 * (should use FOREACH macros rather than individual routines)
 */
/* iterate through all known options */
#define FOREACH_OPTION(i)	\
	for (i = first_option(); !no_more_options(); i = next_option())
extern int first_option (void);
extern int next_option (void);
extern boolean no_more_options (void);

/* iterate through options in option-combination-list */
#define FOREACH_OPTION_IN_COMBO(i,c)	\
	for (i = first_combo_item(c); !no_more_combo_items(c); i = next_combo_item(c))
extern int first_combo_item (int combo_flag);
extern int next_combo_item (int combo_flag);
extern boolean no_more_combo_items (int combo_flag);

/* iterate through implied options for a particular option */
#define FOREACH_IMPLIED_OPTION(i,o)	\
	for (i = first_implied_option(o); !no_more_implied_options(o); i = next_implied_option(o))
extern int first_implied_option (int flag);
extern int next_implied_option (int flag);
extern boolean no_more_implied_options (int flag);

/* return name of current implied string;
 * MUST BE INSIDE IMPLIED ITERATOR when calling this routine! */
extern char *get_current_implied_name (void);

