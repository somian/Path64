/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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

extern char *option_name;	/* current option name */

extern char *optargs;          /* string argument following fixed prefix */
extern int optargd;             /* number argument following fixed prefix */

/* add arg to prefix for indirect option */
extern int add_string_option (int flag, char *arg);
extern int add_string_option_or_dash (int flag, char *arg);

/* iteratively get a single option from the command line */
extern int get_option(int *argi, char *argv[]);

/* if an alias option, return the base "real" option */
extern int get_real_option_if_aliased (int flag);

/* do initial pass through args to check for options that affect driver */
extern void check_for_driver_controls (int argc, char *argv[]);

extern string_list_t *feedback_files;

/* explicitly set language */
void set_explicit_lang(const char *flag, const char *lang);

#ifndef HUGEPAGE_DEF
#define HUGEPAGE_DEF
typedef enum 
{
    SIZE_NA = 0,
    SIZE_2M,
    SIZE_1G,
    SIZE_2M1G,  /* add a new entry before SIZE_END, append its name in the name table below */
    SIZE_END
} HUGEPAGE_SIZE;

/* name table */
static const char * hugepage_size_name[SIZE_END] = {"NA", "2M", "1G", "2M1G"};

/* huge page allocation type */

typedef enum
{
    ALLOC_NA = 0,
    ALLOC_HEAP,
    ALLOC_BDT,
    ALLOC_BSS,  /* add a new entry before ALLOC_END, then append its name in the name table below */
    ALLOC_END
} HUGEPAGE_ALLOC;

/* name table */
static const char * hugepage_alloc_name[ALLOC_END] = {"NA", "HEAP", "BDT", "BSS"};

typedef struct hugepage_desc_tag {
    HUGEPAGE_ALLOC alloc;
    HUGEPAGE_SIZE  size;
    int            limit;
    struct hugepage_desc_tag * next;
} HUGEPAGE_DESC_TAG;

typedef HUGEPAGE_DESC_TAG * HUGEPAGE_DESC;
    
extern HUGEPAGE_DESC hugepage_desc;

/* hugepage default values */

#define HUGEPAGE_ALLOC_DEFAULT ALLOC_HEAP
#define HUGEPAGE_SIZE_DEFAULT  SIZE_2M
#define HUGEPAGE_LIMIT_DEFAULT -1

/* This has to be in sync with bit mask definitions from modified libhugetlbfs */


#define HEAP_BEGIN  3

#define HEAP_2M_BIT HEAP_BEGIN
#define HEAP_1G_BIT (HEAP_BEGIN + 1)

#endif
