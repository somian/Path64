/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>
#include <string.h>
#include "basic.h"

/* copy string and allocate memory for it */
extern char *string_copy (const char *s);

/* make a string starting at position i, and extending for len chars */
extern char *substring_copy (const char *s, int i, int len);

/* concatenate two strings */
extern char *concat_strings (const char *a, const char *b);

/* return suffix of string */
extern char *get_suffix (const char *s);

/* change suffix of string */
extern char *change_suffix (char *s, const char *suffix);

/* string has blank space */
extern boolean has_blank (const char *s);

/* Expand template with arg.
 * Basically, the arg string replaces the %* string in the template,
 * e.g. -foo%s with bar becomes -foobar.
 */
extern char *expand_template_string (const char *template, const char *arg);

/* return true if strings are identical */
#define same_string_prefix(x,y)	(strncmp(x,y,strlen(y)) == 0)

/* Replace old_pattern in base string with new_pattern */
/* This modifies the base string in place */
extern void replace_substring (char *base, const char *old_pattern,
			       const char *new_pattern);


/* linked list of strings */
typedef struct string_item_rec {
	char *name;
	struct string_item_rec *next;
} string_item_t;
typedef struct string_list_rec {
	string_item_t *head;
	string_item_t *tail;
} string_list_t;

/* must call this before using a string list */
extern string_list_t * init_string_list (void);

/* return number of elements in list */
extern int string_list_size(const string_list_t *l);

/* add string to end of list */
extern void add_string (string_list_t *list, const char *s);

/* add string after item in list */
extern void add_after_string (string_list_t *list, string_item_t *item, char *s);

/* add each blank-separated substring to end of list */
#ifndef KEY
extern void add_multi_strings (string_list_t *list, const char *s);
#else
extern void add_multi_strings (string_list_t *list, const char *s,
			       boolean only_one);
#endif

/* add string to end of list if not already in list */
extern void add_string_if_new (string_list_t *list, const char *s);

/* replace old in list with new */
extern void replace_string (string_list_t *list,
			    const char *old, const char *new);

/* append string list b at end of string list a */
extern void append_string_lists (string_list_t *a, string_list_t *b);

/* print the list */
extern void print_string_list (FILE *f, string_list_t *list);

extern const char *ends_with(const char *s, const char *sfx);

/* iterator */
#define FOREACH_STRING(p,list)	\
	for (p = list->head; p != NULL; p = p->next)
#define STRING_NAME(p)	(p->name)

/* linked list of string pairs */
typedef struct string_pair_item_rec {
	char *key;
	char *val;
	struct string_pair_item_rec *next;
} string_pair_item_t;
typedef struct string_pair_list_rec {
	string_pair_item_t *head;
	string_pair_item_t *tail;
} string_pair_list_t;

/* must call this before using a string pair list */
extern string_pair_list_t * init_string_pair_list (void);

/* add string pair to end of list */
extern void add_string_pair (string_pair_list_t *list,
			     const char *key, const char *val);

/* iterator */
#define FOREACH_STRING_PAIR(p,list)	\
	for (p = list->head; p != NULL; p = p->next)
#define STRING_PAIR_KEY(p)	(p->key)
#define STRING_PAIR_VAL(p)	(p->val)

#endif
