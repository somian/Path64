/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


/* USMID @(#) libf/pxf/pxfstructtable.h	92.0	10/08/98 14:30:10 */

#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <grp.h>
#include <utime.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/signal.h>
#include <sys/termios.h>

struct strtble {
        char *str;	/* name of constant */
        int  size;	/* Size of the structure to be allocated */
	int type;	/* This value is stored in the pxftype field */
			/* of the structure returned. */
};

struct sig_set {
	sigset_t	samask;
};

static
struct strtble strtble[] = {
        {"flock",sizeof(struct flock),PXF_FLOCK},
        {"utimbuf",sizeof(struct utimbuf),PXF_UTIMBUF},
        {"utsname",sizeof(struct utsname),PXF_UNAMBUF},
        {"stat",sizeof(struct stat),PXF_STATBUF},
	{"tms",sizeof(struct tms),PXF_TMSBUF},
	{"group",sizeof(struct group),PXF_GROUP},
	{"passwd",sizeof(struct passwd),PXF_PASSWD},
	{"dirent",sizeof(struct dirent),PXF_DIRENT},
	{"sigset",sizeof(struct sig_set),PXF_SIGSET},
	{"sigaction",sizeof(struct sigaction),PXF_SIGACTION},
	{"termios",sizeof(struct termios),PXF_TERMIOS},
	{"subhand",sizeof(long),PXF_SUBHAND},
};
