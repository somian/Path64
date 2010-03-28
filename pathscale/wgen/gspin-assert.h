/*
 * Copyright (C) 2006. PathScale Inc. All Rights Reserved.
 */

/*

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/
                                                                                                                           
#ifndef __GSPIN_ASSERT_H__
#define __GSPIN_ASSERT_H__

extern void gs_assert(bool condition, 
                            gs_string_t file, 
                            gs_string_t function, 
                            gs_count_t line, 
                            const gs_string_t message);

#ifdef Is_True_On
#define GS_ASSERT(condition, message) \
  gs_assert ((condition), (gs_string_t) __FILE__, (gs_string_t) __FUNCTION__, __LINE__, (gs_string_t) (message))
#else
#define GS_ASSERT(condition, message)
#endif

#endif // __GSPIN_ASSERT_H__
