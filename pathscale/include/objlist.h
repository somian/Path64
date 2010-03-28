/*i

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


#ifndef __OBJLIST_H
#define __OBJLIST_H
/* $Header$ */

typedef struct {
        Elf32_Word      oi_magic; /* to distinguish from old obj struct */
	Elf32_Word	oi_size; /* size in bytes of this struct */
	Elf32_Addr	oi_next; /* next record in doubly-linked list */
	Elf32_Addr	oi_prev; /* previous record */
	Elf32_Addr	oi_ehdr; /* address of ELF header in memory */
	Elf32_Addr	oi_orig_ehdr;/* QS address of ELF header */
	Elf32_Addr	oi_pathname; /* pathname as mapped by rld */
	Elf32_Word	oi_pathname_len; /* strlen(pathname) */
} Elf32_Obj_Info;

typedef struct {
        Elf64_Word      oi_magic; /* to distinguish from old obj struct */
	Elf64_Word	oi_size; /* size in bytes of this struct */
	Elf64_Addr	oi_next; /* next record in doubly-linked list */
	Elf64_Addr	oi_prev; /* previous record */
	Elf64_Addr	oi_ehdr; /* address of ELF header in memory */
	Elf64_Addr	oi_orig_ehdr;/* QS address of ELF header */
	Elf64_Addr	oi_pathname; /* pathname as mapped by rld */
	Elf64_Word	oi_pathname_len; /* strlen(pathname) */
} Elf64_Obj_Info;

/* We want dbx, pcs and libexc to be able to tell the new
 * obj struct from the old.  Therefore, since the old one
 * has a pointer in the first 4 bytes, we ensure that the
 * first 4 bytes of the new one cannot be an address.
 * First and foremost, this is accomplished by setting
 * the sign bit, since that indicates kernel space.  I believe
 * that any application will have to know that it is in
 * 64 bit mode before it gets this far.
 */
#define NEW_OBJ_INFO_MAGIC 0xffffffff
#endif

