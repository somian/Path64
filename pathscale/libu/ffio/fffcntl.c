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


#pragma ident "@(#) libu/ffio/fffcntl.c	92.2	10/11/99 15:30:43"

#include <ffio.h>
#include <stdarg.h>
#include <errno.h>

/*
 * ffcntl 
 *
 * Parameters:
 *
 *	fd	- file descriptor (dummy)
 *	cmd	- command code
 *	arg	- command specific parameter
 *	stat	- pointer to status return word
 *
 * Returns:
 *	 0 on success
 *	-1 on failure with either errno or stat.sw_error set to error.
 *
 *
 * Specific cmd code interfaces:
 *
 *	FC_GETINFO      1		arg is of type (struct ffc_info_s *)
 *		Returns FFIO information about the file.
 *
 *	FC_STAT         2		arg is of type (struct stat *)
 *		Emulates an fstat system call.
 *
 *	FC_SETRECL      3
 *		Not implemented
 *
 *	FC_RECALL       4 		arg is of type (struct ffsw *)
 *		Emulates a recall() system call.   Waits for completion of
 *		asynchronous I/O started using ffreada, ffwritea, or 
 *		fflistio with ffsw status word arg. This
 *		command causes delay until the I/O request has completed.
 *		(Note that signals may cause early return with error code
 *		EINTR).
 *
 *	FC_ACPTBAD      5		arg is of type (struct ffc_baddata_s *)
 *		Causes acceptance of bad data by a tape file. 
 *
 *	FC_SKIPBAD      6		arg is of type (struct ffc_baddata_s *)
 *		Causes skipping of bad data by a tape file.  
 *
 *	FC_GETTP        7		arg is of type (struct ffc_gettp_s *)
 *		Gets tape volume position for a tape file.
 *	
 *	FC_AUTOBAD      8		arg is of type int
 *		Sets automatic bad data handling for tape files.  This
 *		is used when the user has specified  the -d option with
 *		the assign command.
 *
 *		arg values:
 *			AUTO_SKIP	Implements "-d skipbad".   When
 *					bad data is encountered, this
 *					one record of bad data is skipped.
 *
 *			AUTO_SKIPALL	When bad data is encountered, 
 *					the file position is advanced to the
 *					first record boundary which follows
 *					the bad data.
 *					
 *			AUTO_ACPT	Implements "-d acptbad". When
 *					bad data is encountered, it is 
 *					accepted without complaint.
 *
 *	FC_CHECKTP      9		arg is of type (struct ffc_chktp_s *)
 *		Check for end of volume on a tape file.
 *
 *	FC_ENDSP        10		arg is not used
 *		End special EOV processing on a tape file.
 *
 *	FC_STARTSP      11		arg is not used
 *		Start special EOV processing on a tape file.
 *
 *	FC_CLOSEV       12		arg is not used
 *		Switches a tape volume.
 *
 *	FC_SETSP        13		arg is of type int
 *		Initializes/terminates special EOV processing on a tape file.
 *
 *		arg values:
 *			1	Turn BOV/EOV processing on
 *			0	Turn BOV/EOV processing off
 *
 *	FC_ASPOLL       14		arg is of type (struct ffsw *)
 *		Checks for completion of asynchronous I/O started using ffsw 
 *		status word arg.  The FC_ASPOLL command causes a status 
 *		check without waiting for the I/O to complete.
 *		
 *	FC_SCRATCH	15		arg is of type (int *)
 *		Specify that a file is to be deleted at close time.
 *
 *		Sets these flags in *arg:
 *
 *			Flag		Indicates
 *
 *			SCR_SINGLELINK 	Set if the file is not a pipe or a tty,
 *					has a link count equal to one, and is 
 *					not a symbolicly linked file.  
 *					
 *			SCR_UNLINKED	Set if this fffcntl() call has 
 *					successfully unlinked the file.
 *
 *			SCR_NOFLUSH	Set if ffclose() processing has been
 *					optimized to suppress buffer flushing. 
 *
 *		*arg is set to 0 if this request had no effect.
 *	
 *	FC_DUMPSTATS	17
 *		Dumps intermediate statistics if they are being gathered
 */
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
fffcntl(int fd, int cmd, void *arg, struct ffsw *pstat)
#else
fffcntl(int fd, int cmd, ...)
#endif
{
	struct fdinfo *fio;
	int ret, na;
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	int  arg;
	struct ffsw locstat, *pstat;
	va_list ap;
#endif

	fio = GETIOB(fd);
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	va_start(ap, cmd);
	arg = va_arg(ap, int);
#endif

#ifdef	_UNICOS
	NUMARG(na);
#else
	na = 4;
#endif
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	if (na < 4)
		pstat = &locstat;
	else
		pstat = va_arg(ap, struct ffsw *);
#endif

	CHECK_FIOPTR(fio, pstat);
	ret = XRCALL(fio, fcntlrtn) fio, cmd, arg, pstat);
#if	!defined(__mips) && !defined(_LITTLE_ENDIAN)
	if (ret == -1 && na < 4)
		errno = locstat.sw_error;	/* stat arg was not passed */
#endif
	return (ret);
}
