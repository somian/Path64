/*
  Copyright (C) 2006 PathScale Inc. All Rights Reserved.
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

#ifndef __GSPIN_ALLOC_H__
#define __GSPIN_ALLOC_H__
                                                                                                                                                           
////////////////////////////////////////////////////////////////////////////////
//
// Wrappers around calloc (3) and free (3):
//
////////////////////////////////////////////////////////////////////////////////

extern void *gs_alloc (size_t nmemb, size_t size);

#endif // __GSPIN_ALLOC_H__
