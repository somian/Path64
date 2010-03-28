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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#ifndef pair_H
#include <pair.h>
#endif

#ifndef FUNCTION_H
#include <function.h>
#endif

typedef Elf32_Shdr * Shdr32_tag;
typedef Elf64_Shdr * Shdr64_tag;


template <class Shdr>
struct Proc_Iter
{
    char *strtab;
    Shdr *current;
    Shdr *last;

    Proc_Iter (char *str, Shdr *first, Shdr *end)
	: strtab (str), current (first), last (end) {}
    Proc_Iter *operator ++ () {
	++current;
	while (current != last && (current->sh_type != SHT_PROGBITS ||
				   (current->sh_flags & SHF_EXECINSTR) == 0))
	    ++current;
	
	return this;
    }
    
    pair<char *,long> operator * () {
	return make_pair (strtab + current->sh_name, current->sh_size);
    }

}; // Proc_Iter


template <class Shdr>
inline bool
operator == (const Proc_Iter<Shdr> &x, const Proc_Iter<Shdr> &y)
{
    return x.current == y.current;
}

class File_Info
{
private:
    void *mmap_handle;
    long file_size;

    char obj_class;			// ELFCLASS32 or ELFCLASS64
    char *strtab;

    void *shdr_begin;
    void *shdr_end;

    template <class HEADER, class SHDR>
	void fill_file_info (HEADER *ehdr, SHDR *shdr);
    
    inline void process_file ();
public:

    File_Info (char *name);

    ~File_Info ()		{ munmap (mmap_handle, file_size); }

    char Get_obj_class () const	{ return obj_class; }

    template <class Shdr>
	Proc_Iter<Shdr> begin (Shdr *shdr) const {
	    return Proc_Iter<Shdr> (strtab, (Shdr *) shdr_begin,
				    (Shdr *) shdr_end);
	}

    template <class Shdr>
	Proc_Iter<Shdr> end (Shdr *shdr) const {
	    return Proc_Iter<Shdr> (strtab, (Shdr *) shdr_end,
				    (Shdr *) shdr_end);
	}
}; // File_Info


