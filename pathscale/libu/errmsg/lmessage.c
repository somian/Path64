/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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
#ifdef KEY
/* DON'T ADD ANY #include HERE */
/* On Fedora Core 5, with gcc 4.1.1 installed, and "-D_XOPEN_SOURCE{,_EXTENDED}"
 * on the command line, "pathcc -gnu40" gives an "implicit declaration" error
 * for snprintf. Using -std=c99 fixes the problem, but applying that to the
 * entire libu requires zillions of fixes to obsolete C idioms (e.g., implicit
 * "int" typing of functions) plus a couple of FP-related changes that might
 * break things. I have no idea why _XOPEN_SOURCE and _XOPEN_SOURCE_EXTENDED
 * are required (perhaps stuff related to "struct timeval" in other source
 * files?) but we don't want it here.
 */
#undef _XOPEN_SOURCE
#undef _XOPEN_SOURCE_EXTENDED
#endif /* KEY */

#pragma ident "@(#) libu/errmsg/lmessage.c	92.4	10/14/99 17:05:30"
#include <liberrno.h>
#include <errno.h>
#include <fcntl.h>
#if !defined(_ABSOFT)
#include <nl_types.h>
#else
#include "ac_msg_type.h"
#endif
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#if defined(_ABSOFT) && defined(TARGET_NT)
#include <siohdr.h>
#endif
#if	defined(_LITTLE_ENDIAN)
#include <cray/nlcatmsg.h>
#endif
#include <unistd.h>

#ifdef	_UNICOS

#include <tapereq.h>

extern	char	**_argv;
#define	CMDNAME	(*_argv)

#else

#include <cray/portdefs.h>
#if	defined(_LITTLE_ENDIAN)
#include <cray/nlcatmsg.h>
#endif

#define	CMDNAME	""

#endif

#ifdef KEY /* Bug 6673 */
#include <stdlib.h>
#include <libgen.h>

/* User sets this variable to request verbose messages */
#define VERBOSITY_ENVIRONMENT_VARNAME "PSC_ERR_VERBOSE"

/* Command to find absolute path to lib.msgs */
#  define PFN_COMMAND "pathf95 -print-file-name=%s.msgs"

/* Command to format verbose message output. Set GROFF_TMAC_PATH so nroff can
 * find the tmac.sg macro file (assumed to be in the same directory as
 * lib.msgs); filter with "cat -s" because the nroff produces unwanted trailing
 * blank lines */
#  define NROFF_COMMAND "GROFF_TMAC_PATH=%s /usr/bin/nroff -msg | /bin/cat -s"

/* Line prefixed with this string is the beginning of the verbose version
 * of a particular message within lib.msgs */
#  define NEXP "$nexp"

int snprintf(char *str, size_t size, const char *format, ...);

/*
 * Initialize variables for use by verbose_message()
 * mcnm		Base name of message file (e.g. "lib")
 * msgs_file	File descriptor for lib.msgs
 * nroff_cmd	Command to format message output
 * return	1 on success, 0 otherwise
 */
static int
verbose_init(char *mcnm, FILE **msgs_file, char **nroff_cmd) {
  char pfn_buf[(sizeof PFN_COMMAND) + FILENAME_MAX];
  if (0 > snprintf(pfn_buf, sizeof pfn_buf, PFN_COMMAND, mcnm)) {
    return 0;
  }
  FILE *pfn = popen(pfn_buf, "r");
  char *err = fgets(pfn_buf, sizeof pfn_buf, pfn);
  pclose(pfn);
  if (NULL == err) {
    return 0;
  }
  char *nl = strchr(pfn_buf, '\n');
  nl ? (*nl = 0) : '\0';
  if (NULL == (*msgs_file = fopen(pfn_buf, "r"))) {
    return 0;
  }
  char *pfn_dirname = dirname(pfn_buf);
  unsigned dirname_len = strlen(pfn_dirname);
  int nc_len = (sizeof NROFF_COMMAND) + dirname_len;
  *nroff_cmd = malloc(nc_len);
  if (0 == *nroff_cmd ||
    0 > snprintf(*nroff_cmd, nc_len, NROFF_COMMAND, pfn_dirname)) {
    return 0;
  }
  return 1;
}

/*
 * Print verbose version of message using lib.msgs
 *
 * mcnm		Basename of message file (Normally "lib")
 * errn		message number
 * return	1 on success, 0 otherwise
 */
int
verbose_message(char *mcnm, int errn)
{
  static enum {
    verbosity_false = 0,	/* Terse messages */
    verbosity_true = 1,		/* Verbose messages */
    verbosity_unknown		/* Need to initialize "verbosity" */
    } verbosity = verbosity_unknown;
  static FILE *msgs_file = 0; /* lib.msgs */
  static char *nroff_command = 0;

  if (verbosity == verbosity_unknown) {
    verbosity = !!getenv(VERBOSITY_ENVIRONMENT_VARNAME);
    }
  if (verbosity == verbosity_false) {
    return 1;
  }

  if ((!msgs_file) && (!verbose_init(mcnm, &msgs_file, &nroff_command))) {
    return 0;
  }

  char buf[1024];
  fflush(0);

  /* Find verbose part of the message having the desired number */
  rewind(msgs_file);
  for (;;) {
    if (NULL == fgets(buf, sizeof buf, msgs_file)) {
      return 0;
    }
    int number;
    if (sscanf(buf, NEXP " %d", &number) && number == errn) {
      break;
    }
  }

  FILE *nroff = popen(nroff_command, "w");
  if (NULL == nroff) {
    return 0;
  }

  /* Read verbose message and write output, skipping first line of verbose
   * message, which is redundant with the terse message */
  int status = 0;
  int not_first = 0;
  for (;; not_first = 1) {
    if (NULL == fgets(buf, sizeof buf, msgs_file)) {
      break;
    }
    if (buf[0] == '$') {
      status = 1;
      break;
    }
    if (not_first && 0 == fwrite(buf, strlen(buf), 1, nroff)) {
      break;
    }
  }

  pclose(nroff);
  return status;
}
#endif /* KEY Bug 6673 */

/*
 *	_lmessage - run-time library message processor
 *
 *		errn		Message number
 *		severity	Pointer to message severity; NULL if
 *				unrecoverable error message
 */

void
_lmessage(int errn, char *severity, va_list args)
{
	register int	flmn;		/* library message number 	*/
	char		mbuf[MAXMLN+1];	/* Message buffer		*/
	char		tbuf[MAXMLN+1];	/* Temporary message buffer	*/
	char		*mcnm;		/* Pointer to msg. catalog name	*/
	char		*bmsg;		/* Pointer to back-up message	*/
	char		*smsg;		/* Pointer to severity text	*/
	nl_catd		mcfd;		/* Message catalog descriptor	*/
#ifdef	__mips
	static nl_catd	lib_mcfd;	/* lib msg cat file descriptor	*/
	static int	lib_mcfd_opnd = 0; /* lib msg cat already open flag */
	int		lib_mcat = 0;	/* lib msg catalog indicator	*/
#endif	/* __mips */

#ifdef	_UNICOS
	if (_numargs() == 0)
		(void) abort();
#endif

	mbuf[0]	= '\n';
	mbuf[1]	= '\0';
	bmsg	= NULL;		/* Assume no back-up message */
	smsg	= "";		/* Assume unset severity */
	flmn	= errn;

	/*
	 * Map -1 errors to FERDPEOF (temporarily, we hope)
	 * Map all negative errors to positive errors.
	 */

	if (flmn < 0)
		if (flmn == -1)
			flmn	= FERDPEOF;
		else
			flmn	= -flmn;

	if (flmn < BASE) {
		mcnm	= "sys";
#ifdef KEY /* Bug 4452 */
		smsg	= "UNRECOVERABLE error on system request\nlib-%d: %s";
#else /* KEY Bug 4452 */
		smsg	= "UNRECOVERABLE error on system request";
#endif /* KEY Bug 4452 */
#ifdef	_UNICOS
		/*
		 * Provide a back-up error message for those errors
		 * where we expect that the message system will fail
		 * to retrieve a message.
		 */

		if (flmn == ENOMEM)
			bmsg	= "Unable to allocate memory.";

		if (flmn == EMFILE)
			bmsg	= "Too many open files.";
#else
		/*
		 * Use strerror() to provide a back-up error message
		 * for 'sys' errors on non-UNICOS systems.  We don't
		 * want to load strerror() on UNICOS systems, however.
		 */

		bmsg	= strerror(flmn);
#ifdef KEY /* Bug 4452 */
		/* Write these messages and return, because we won't be able
		 * to find flmn in the message catalog.
		 */
		fprintf(stderr, smsg, flmn, bmsg);
		return;
#endif /* KEY Bug 4452 */
#endif
	}
	else
		if (flmn < (BASE+999) ||
		    (flmn >= FDC_ERRB && flmn < (FDC_ERRB+999))) {
			mcnm	= FEMCN;	/* Library catalog name */
#ifdef	__mips
			lib_mcat	= 1;
#endif	/* __mips */
			smsg	= "UNRECOVERABLE library error";

			/*
			 * Provide a minimal back-up message for
			 * out-of-memory errors.
			 */

			if (flmn == FENOMEMY || flmn == FEFMTMEM ||
			    flmn == FDC_ERR_NOMEM)
				bmsg	= "Unable to allocate memory.";
		}
#ifdef	_UNICOS
		else
			if (flmn >= ETFIRST && flmn <= ETLAST) {
				mcnm	= "tape";
				smsg	= "UNRECOVERABLE tape error";
				bmsg	= "Consult Appendix C, Tape Subsystem User's Guide, SG-2051";
			}
#endif
			else {
				mcnm	= "unknown";
				smsg	= "UNRECOVERABLE";
				bmsg	= "Unrecognized error number";
			}

	if (bmsg != NULL)	/* Set back-up message, if extant */
		(void) strcpy(&mbuf[1], bmsg);

	if (severity != NULL)
		smsg	= severity;

	/* Open the message catalog */
#ifdef	__mips
	/* if this is the library message catalog, then... */ 
	if (lib_mcat) {
		if (lib_mcfd_opnd) {
			/* set lib msg cat fd to opened lib msg cat fd */
			mcfd	= lib_mcfd;
		} else {
			/* message catalog not opened yet, do the following:
			 * 1. open it
			 * 2. save the lib msg cat fd
			 * 3. set flag indicating it was opened
			 */
			mcfd	= catopen(mcnm, 0);
			lib_mcfd	= mcfd;
			lib_mcfd_opnd	= 1;
		}
	} else {
		/* not the lib message catalog, open catalog */
		mcfd	= catopen(mcnm, 0);
	}
#else	/* __mips */
	mcfd	= catopen(mcnm, 0);
#endif	/* __mips */

	/* Retrieve the raw message text */

#ifdef KEY /* bug 6682 */
	/* If we can't get the NLS message, print something lest the user
	 * think that the boilerplate surrounding the missing message (e.g.
	 * "Unit n not connected") is the real problem. Usual cause is a
	 * C-coded "main" which hides the Fortran runtime "main".
	 */
	char *cgm_result;
	if (((nl_catd) -1) == mcfd ||
	  0 ==
	    (cgm_result = catgetmsg(mcfd, NL_MSGSET, flmn, &mbuf[1], MAXMLN)) ||
	  0 == *cgm_result) {
	  strcpy(mbuf,
	    "\nUnable to find error message (check NLSPATH, file lib.cat)");
	}
#else /* KEY bug 6682 */
	(void) catgetmsg(mcfd, NL_MSGSET, flmn, &mbuf[1], MAXMLN);
#endif /* KEY bug 6682 */

	/* Edit the message */

	(void) vsprintf(tbuf, &mbuf[1], args);

	/* Format the message */

	(void) catmsgfmt(CMDNAME, mcnm, flmn, smsg, tbuf, &mbuf[1],
			 MAXMLN, NULL, NULL);

	/* Print the formatted message */

#if !defined(_ABSOFT)
	(void) write(fileno(stderr), mbuf, strlen(mbuf));
#else
	fwrite(mbuf, strlen(mbuf), 1, stderr);
#endif

#ifdef KEY /* Bug 6673 */
	/* If user wants verbose message, print it now. We always print the
	 * terse message first because it might have substitutable arguments
	 * which add meaning.  We skip the first line of the verbose message
	 * because it's the same as the terse message, with constant lexemes
	 * in place of the substitutable arguments. */
	if (cgm_result && *cgm_result) {
	  verbose_message(mcnm, errn);
	}
#endif /* KEY Bug 6673 */

	return;
}
