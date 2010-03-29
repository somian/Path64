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

#ifndef __GSPIN_IO_H__
#define __GSPIN_IO_H__

extern gs_void_t gs_write (const gs_string_t filename);
extern gs_unsigned_char_t *gs_read (const gs_string_t filename);
extern gs_t gs_read_file (const gs_string_t filename);

#endif // __GSPIN_IO_H__
