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



#ifndef PATH_INTRINSIC_LIST_INCLUDED
#define PATH_INTRINSIC_LIST_INCLUDED

#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  char *name_; /* Name of intrinsic */
  char added_; /* True if we should add, false if we should delete */
  char isfamily_; /* True if this is a family, not an individual intrinsic */
  } intrin_option_t;

extern int path_intrinsic_list_length();

extern intrin_option_t **path_intrinsic_list_list();

#ifdef __cplusplus
}

// Maintain a list of intrinsics to be added to the baseline or deleted
// from the baseline. We are responsible only for the list: the
// "enter_intrinsic_info" function in fe90/p_driver.c establishes the
// baseline list and adds to that list or deletes from it.
class intrinsic_list_t {

  static int list_len_;
  static int list_capacity_;
  static intrin_option_t **list_;

  public:

    // Add an intrinsic name to the list we maintain, marking whether it
    // should be added to or deleted from the baseline.
    static void add(char *name, int added);

    static int length() { return list_len_; }

    static intrin_option_t **list() { return list_; }
  };

extern intrinsic_list_t path_intrinsic_list;

#endif /* __cplusplus */

#endif /* PATH_INTRINSIC_LIST_INCLUDED */

