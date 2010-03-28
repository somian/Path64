/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/unitclose.c	92.2	06/18/99 18:38:26"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <liberrno.h>
#include <cray/nassert.h>
#include <sys/param.h>
#include <sys/stat.h>
#include "fio.h"

extern short	_f_abort;	/* Abort flag - set in _f_sig() */

/*
 * 	_unit_close
 *
 *		Closes a Fortran unit.  On exit, the unit is closed and 
 *		optionally deallocated with the appropriate locks unlocked.
 *
 *		Upon entry, the unit pointer passed in must be locked, and
 *		the _openlock word must be locked.
 *
 *		This routine also posts CLOSE statistics for the unit if
 *		statistics gathering is active.
 *
 *	Arguments
 *
 *		cup	- Unit pointer
 *		cstat	- STATUS= specifier--CLST_UNSPEC, CLST_KEEP, or
 *			  CLST_DELETE.
 *
 *	Return value:
 *		 0 	normal return
 *		>0 	system or library error status
 */

int
_unit_close(
	unit	*cup,	/* Unit table pointer */
	int	cstat,	/* STATUS= specifier value on CLOSE */
	FIOSPTR	cssa)	/* non-NULL if called from _CLS() for explicit CLOSE */
{
	register short	delete;	/* 1 if file should be deleted		*/
	register int	ret;	/* Return status			*/
	register int	errn;	/* Status from low-level close operation*/
	plock_t		*lockp;	/* Copy of auxiliary lock pointer	*/
	struct ffsw	fst;	/* ffclose() status			*/
	struct fiostate	state;	/* Local Fortran I/O state structure 	*/
	FIOSPTR		css; 

	if (cup == NULL)	/* If unit not connected */
		return(0);	

	assert ( cstat == CLST_UNSPEC ||
		 cstat ==   CLST_KEEP ||
		 cstat == CLST_DELETE );

	if (cssa != NULL)	/* If processing an explicit CLOSE */
		css	= cssa;
	else {			/* If implicit/automatic close */
		(void) memset(&state, 0, sizeof(state)); /* be extra safe */
		css	= &state;
#ifdef	_UNICOS
		css->f_rtbgn	= _rtc();
#endif
	}

	cup->uend	= BEFORE_ENDFILE;
	errn		= 0;
	delete		= (cstat == CLST_DELETE || cup->uscrtch) ? 1 : 0;

	lockp	= cup->auxlockp; /* copy aux lock pointer for later */

        if (delete && !cup->utmpfil) {
                ret	= _unit_scratch(cup);
		errn	= errn ? errn : ret;
        }

/*
 *	Write out the current record if the last I/O was a nonadvancing WRITE.
 */
	if (cup->pnonadv) {		/* There is a current record */
		if (cup->uwrt) {
			ret	= _nonadv_endrec(css, cup);
			errn	= errn ? errn : ret;
		}
		cup->pnonadv	= 0;	/* Flag no current record */
	}

/*
 *	Truncate sequential files after last write if cup->utrunc.
 */
	if (cup->useq && cup->uwrt && cup->utrunc) {
		ret	= _unit_trunc(cup);
		errn	= errn ? errn : ret;
	}

/*
 *	Perform the close of the lower level file structure only if
 *	it is not a standard file.
 */
	if ( ! ((cup->ufs == STD || cup->ufs == FS_TEXT) &&
		(cup->ufp.std == stdin ||
		 cup->ufp.std == stdout ||
		 cup->ufp.std == stderr)) ) {

		switch (cup->ufs) {

		case FS_FDC:
			if ( !_f_abort || !(cup->uflagword & FFC_NOCLOSE))
				if (__ffclose(cup->ufp.fdc, &fst) < 0)
					errn	= errn ? errn : fst.sw_error;
			break;

		case FS_TEXT:
		case STD:
				
			if (fclose(cup->ufp.std) != 0)
				errn	= errn ? errn : errno;
			break;

		case FS_AUX:
			errn	= errn ? errn : FEMIXAUX;

		default:
			errn	= errn ? errn : FEINTFST; /* Unknown file type */
		} /* switch */
	}

/*
 *	Unlock the lockp word.   This is a pointer to a lock word
 *	shared by another unit, usually cup->ufp.std->_lock.  
 */
	if (lockp != NULL)
		MEM_UNLOCK(lockp);

/*
 *	Post final statistics and deallocate the statistics packet.
 */
	FSTATS_POST(cup, T_CLOSE, css);	/* Post CLOSE statistics */

	_ft_stclose(cup);		/* Send close packet to procstat
					 * and then deallocate it.	*/

/* 
 *	Deallocation of structures pointed to by the unit table follows
 */
	if (cup->ulinebuf != NULL)
		free(cup->ulinebuf);

	if (cup->upfcstk != NULL)
		free(cup->upfcstk);

	if (cup->urepdata != NULL)
		free(cup->urepdata);

/*
 *	Free ufnm and alfnm.  
 */
	if (cup->ufnm != NULL)
		free(cup->ufnm);

	if (cup->alfnm != NULL)
		free(cup->alfnm);
 
/*
 *	Mark the unit as unopen.
 */
	cup->ufs	= 0;

	return(errn);
}
  
/*
 *	_unit_scratch()
 *
 *		Unlink the file associated with a unit.  For some file types, 
 *		this operation affects subsequent close processing so as to
 *		suppress buffer flushing before the close.   
 *
 *	Return value
 *		0  on success
 *		>1 on error
 *
 *	Side effects
 *
 *		cup->uscrtch	is set to 1.
 *		cup->unlinked	is set to 1 if the file was unlinked.
 *		cup->usnglink	is set to 1 if the file is not a symbolic link 
 *				and the file link count is 1.
 *
 *		The optimization in close processing to suppress buffer flushes
 *		may only occur if (cup->unlinked && cup->usnglink).
 */
int 
_unit_scratch(unit *cup)
{
	register int	ret;

	cup->uscrtch	= 1;

#ifdef	FC_SCRATCH
        if (cup->ufs == FS_FDC) {
		int		scrflags;
		struct ffsw	ffst;

		cup->unlinked	= 0;
		cup->usnglink	= 0;


		/*
		 * Use the FC_SCRATCH request to unlink the file, check if it 
		 * is a linked file and possibly setup close processing to 
		 * suppress buffer flushes.
		 */
		ret	= XRCALL(cup->ufp.fdc, fcntlrtn) cup->ufp.fdc,
			FC_SCRATCH, &scrflags, &ffst);

		if (ret == 0) {
			cup->unlinked	= (scrflags & SCR_UNLINKED)   ? 1 : 0;
			cup->usnglink	= (scrflags & SCR_SINGLELINK) ? 1 : 0;
			return(0);
		}

		/* Fall through on error;  FC_SCRATCH might not be supported */
	}
#endif

	cup->usnglink	= 0;	/* assume multiple links */

/*
 *	Perform the unlink only if connected to a real file.  Permit ENOENT
 *	errors in case a layer like mr.scr had closed the file at open time
 *	even though STATUS='SCRATCH' might not have been specified on the OPEN.
 *
 *	If the file has already been unlinked, or if the file has been renamed
 *	and a different file is now there with the same name, ignore the error.
 */
	if (cup->alfnm != NULL && cup->usysfd >= 0) {
		struct stat	sbuf;

		ret	= stat(cup->alfnm, &sbuf);

		if (ret != -1) {	/* If not already unlinked */

			/*
			 * Ensure the device and inode number match before
			 * we pull the trigger.
			 */

			if (sbuf.st_ino != cup->uinode ||
			    sbuf.st_dev != cup->udevice)
				return(FENODELT);	/* Can't delete file */
			else {

				ret	= unlink(cup->alfnm);

				if (ret == -1 && errno != ENOENT)
					return(errno);
			}
		}

		cup->unlinked	= 1;
	}

	return(0);
}
