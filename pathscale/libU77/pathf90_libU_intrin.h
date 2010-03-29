/*
Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.

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

#include "cray/mtlock.h"

/*
 * The "pathf90_xxx" intrinsics provide implementations which do not interfere
 * with the Fortran namespace, in which symbols end in "_". For backward
 * compatibility, we provide legacy versions (in files legacy_xxx_.c) which
 * call the "pathf90_xxx" versions. They are in separate .o files so that
 * conflicts with user-written procedures don't occur when linking statically.
 */

/* Data types chosen to match Fortran for both -m32 and -m64 */
typedef int pathf90_i4; /* integer*4 */
typedef long long pathf90_i8; /* integer*8 */
typedef float pathf90_r4; /* real*4 */
typedef double pathf90_r8; /* real*8 */

extern void pathf90_abort(void);
extern pathf90_i4 pathf90_access(char *, char *, int, int);
extern pathf90_i4 pathf90_alarm(pathf90_i4 *,void (*)(), pathf90_i4 *);
extern pathf90_i4 pathf90_chdir(char *, pathf90_i4 *, int);
extern pathf90_i4 pathf90_chmod(char *, char *, pathf90_i4 *, int, int);
extern void pathf90_ctime4(char *, int, pathf90_i4 *);
extern void pathf90_ctime8(char *, int, pathf90_i8 *);
extern void pathf90_subr_ctime4(pathf90_i4 *, char *, int);
extern void pathf90_subr_ctime8(pathf90_i8 *, char *, int);
extern pathf90_r4 pathf90_dtime(pathf90_r4 tarray[2]);
extern pathf90_r4 pathf90_etime(pathf90_r4 tarray[2]);
extern void pathf90_fdate(char *, int); 
extern void pathf90_getarg(pathf90_i4 *, char*, int);
extern pathf90_i4 pathf90_getcwd(char *, pathf90_i4 *, int);
extern void pathf90_getenv(char *, char *, int, int);
extern pathf90_i4 pathf90_getgid(void);
extern void pathf90_getlog(char *, int);
extern pathf90_i4 pathf90_getpid(void);
extern pathf90_i4 pathf90_getuid(void);
extern void pathf90_gmtime(pathf90_i4 *, pathf90_i4 *);
extern pathf90_i4 pathf90_hostnm(char *, pathf90_i4 *, int);
extern pathf90_i4 pathf90_iargc(void);
extern void pathf90_itime(pathf90_i4 *);
extern void pathf90_ltime(pathf90_i4 *, pathf90_i4 *);
extern pathf90_i4 pathf90_rename(char *, char *, pathf90_i4 *, int, int);
extern void pathf90_sleep(pathf90_i4 *);
extern pathf90_i4 pathf90_stat(char *, pathf90_i4 *, pathf90_i4 *, int);
extern pathf90_i4 pathf90_symlnk(char *, char *, pathf90_i4 *, int, int);
extern pathf90_i4 pathf90_system(char *, pathf90_i4 *, int);
extern pathf90_i4 pathf90_time4(void);
extern pathf90_i8 pathf90_time8(void);
extern pathf90_i4 pathf90_unlink(char *, pathf90_i4 *, int);

extern plock_t pathf90_rand_mutex;
