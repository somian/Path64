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


#ifndef __MAKE_DEPEND_H__
#define __MAKE_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * An opaque handle on the structure used to update a single target's make
 * dependencies rule.  Each target built in a directory has one rule in an
 * automatically-generated make-dependencies file, usually named Makedepend
 * and sincluded by the directory's makefile.  Each rule relates the target
 * to all source files that it #includes and all libraries/runtimes that it
 * links against.
 */
typedef struct mdrule *MDhandle;

/*
 * Call MDopen(toolname, filename, target, errorfunc) to open a make rule
 * for target in filename.  Target may be null if the calling tool does not
 * yet know its output file's name.  Errorfunc is called for malloc failure
 * and system call errors.  Toolname is used to distinguish link-editor and
 * include-file dependencies for the same target (e.g., a command made using
 * a null-suffix rule).
 *
 * MDupdate(handle, dependency) adds the named dependency to the right part
 * of target's rule.  Dependency must be in persistent store.
 *
 * MDclose(handle, target) updates the dependencies file named by MDopen's
 * filename argument.  If a null target was passed to MDopen, a non-null one
 * must be passed to MDclose.  MDclose locks filename, updates target's rule
 * in it or adds a new rule if target had none, and unlocks filename.  Lines
 * in filename not matching '^target *:' are left alone.
 */
extern MDhandle	MDopen(const char *, const char *, const char *, void (*)(const char *, ...));
extern void	MDupdate(MDhandle, char *);
extern void	MDclose(MDhandle, char *);

#ifdef __cplusplus
}
#endif

#endif /* __MAKE_DEPEND_H__ */
