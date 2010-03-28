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


#pragma ident "@(#) libf/pxf/pxfgetpwuid.c	92.3	10/29/99 21:41:49"


#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#ifdef	_LITTLE_ENDIAN
#include <sys/types.h>
#endif
#include "pxfstruct.h"
#include "table.h"


/*
 *  PXFGETPWUID  -- get password information using user ID
 *  (section 9.2.2 of Posix 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *     SUBROUTINE PXFGETPWUID(IUID,JPASSWD,IERROR)
 *     INTEGER IUID,JPASSWD,IERROR
 *
 *  Where:
 *
 *  IUID     is an input integer variable containing the user ID for which
 *           password information is desired.
 *
 *  JPASSWD  is a output handle of type 'passwd' created with
 *           PXFSTRUCTCREATE.
 *
 *  IERROR   is an output integer variable that will contain
 *           the status:
 *
 *            zero    - PXFGETPWUID was successful.
 *
 *            nonzero - PXFGETPWUID was not successful.
 *
 *           PXFGETPWUID may return any of the following error
 *           values:
 *
 *            ENOENT If an entry matching the user ID in IUID is not found.
 * 
 *            EBADHANDLE If JPASSWD is invalid.
 *
 */


#ifdef _UNICOS
void
PXFGETPWUID(
#else
void
_PXFGETPWUID(
#endif
	     _f_int *IUID,
	     _f_int *JPASSWD,
	     _f_int *IERROR
)
{
  char *cname;
  struct passwd *passwdsrc, passwdtemp, *cjpasswd;
  struct pxfhandle pxfhand;

  pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *JPASSWD);
  if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_PASSWD) {
    *IERROR = EBADHANDLE;
    return;
  }
  cjpasswd = (struct passwd *)pxfhand.pxfstructptr;

  /* make call to getpwuid */
  if ((passwdsrc = getpwuid((uid_t)*IUID)) != NULL) {
    
    /* copy the structures components since static storage is used */
    /* component: pw_name (login name) */
    passwdtemp.pw_name =
      (char *) malloc((strlen(passwdsrc->pw_name)+1)*sizeof(char));
    if (passwdtemp.pw_name == NULL) {
      *IERROR = ENOMEM;
      return;
    }
    (void)strcpy(passwdtemp.pw_name, passwdsrc->pw_name);
    
    /* component: pw_uid (user ID) */
    passwdtemp.pw_uid = passwdsrc->pw_uid;
    
    /* component: pw_gid (group ID) */
    passwdtemp.pw_gid = passwdsrc->pw_gid;
    
    /* component: pw_dir (default login directory) */
    passwdtemp.pw_dir =
      (char *) malloc((strlen(passwdsrc->pw_dir)+1)*sizeof(char));
    if (passwdtemp.pw_dir == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      return;
    }
    (void)strcpy(passwdtemp.pw_dir, passwdsrc->pw_dir);
    
    /* component: pw_shell (default login shell) */
    passwdtemp.pw_shell =
      (char *) malloc((strlen(passwdsrc->pw_shell)+1)*sizeof(char));
    if (passwdtemp.pw_shell == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      free(passwdtemp.pw_dir);
      return;
    }
    (void)strcpy(passwdtemp.pw_shell, passwdsrc->pw_shell);
    
    /* components not supported in Posix 1003.9-1992, but supported in target OSes */
    /* component: pw_passwd (encrypted password) */
    passwdtemp.pw_passwd =
      (char *)malloc((strlen(passwdsrc->pw_passwd)+1)*sizeof(char));
    if (passwdtemp.pw_passwd == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      free(passwdtemp.pw_dir);
      free(passwdtemp.pw_shell);
      return;
    }
    (void)strcpy(passwdtemp.pw_passwd, passwdsrc->pw_passwd);
    
#ifndef	_LITTLE_ENDIAN
    /* component: pw_age (password age) */
    passwdtemp.pw_age =
      (char *)malloc((strlen(passwdsrc->pw_age)+1)*sizeof(char));
    if (passwdtemp.pw_age == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      free(passwdtemp.pw_dir);
      free(passwdtemp.pw_shell);
      free(passwdtemp.pw_passwd);
      return;
    }
    (void)strcpy(passwdtemp.pw_age, passwdsrc->pw_age);
    
    /* component: pw_comment (comment) */
    passwdtemp.pw_comment =
      (char *)malloc((strlen(passwdsrc->pw_comment)+1)*sizeof(char));
    if (passwdtemp.pw_comment == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      free(passwdtemp.pw_dir);
      free(passwdtemp.pw_shell);
      free(passwdtemp.pw_passwd);
      free(passwdtemp.pw_age);
      return;
    }
    (void)strcpy(passwdtemp.pw_comment, passwdsrc->pw_comment);
#endif	/* not _LITTLE_ENDIAN */
    
    /* component: pw_gecos */
    passwdtemp.pw_gecos =
      (char *)malloc((strlen(passwdsrc->pw_gecos)+1)*sizeof(char));
    if (passwdtemp.pw_gecos == NULL) {
      *IERROR = ENOMEM;
      free(passwdtemp.pw_name);
      free(passwdtemp.pw_dir);
      free(passwdtemp.pw_shell);
      free(passwdtemp.pw_passwd);
#ifndef	_LITTLE_ENDIAN
      free(passwdtemp.pw_age);
#endif	/* not _LITTLE_ENDIAN */
      free(passwdtemp.pw_gecos);
      return;
    }
    (void)strcpy(passwdtemp.pw_gecos, passwdsrc->pw_gecos);
    
  } else {
    *IERROR = errno;
    return;
  }
  
  /* free all components for the jpasswd handle. NOTE: free() as defined in ANSI C
   * checks for a NULL pointer so this extra check does not need to be performed. */
  free(cjpasswd->pw_name);
  free(cjpasswd->pw_passwd);
#ifndef	_LITTLE_ENDIAN
  free(cjpasswd->pw_age);
  free(cjpasswd->pw_comment);
#endif	/* not _LITTLE_ENDIAN */
  free(cjpasswd->pw_gecos);
  free(cjpasswd->pw_dir);
  free(cjpasswd->pw_shell);
  
  *cjpasswd = passwdtemp;
}

#ifndef _UNICOS
void
pxfgetpwuid_(
	     _f_int *IUID,
	     _f_int *JPASSWD,
	     _f_int *IERROR
)
{
  _PXFGETPWUID(IUID,JPASSWD,IERROR);
}
#endif
