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


#pragma ident "@(#) libf/pxf/pxfsetenv.c	92.1	06/29/99 11:36:06"



#include <stdlib.h>
#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>

#ifndef _UNICOS
#include <stddef.h>
#endif

extern char *_fc_acopy(_fcd f);

/*
 *  PXFSETENV  -- set environment variable pair
 *  (section 4.6.1 of POSIX 1003.9-1992)
 *
 *  Call from Fortran:
 *
 *	SUBROUTINE PXFSETENV(NAME, LENNAME, NEW, LENNEW, IOVERWRITE, IERROR)
 *	CHARACTER*(*) NAME, NEW
 *	INTEGER LENNAME, LENNEW, IOVERWRITE, IERROR
 *
 *  Where:
 *
 *  NAME        is an input character variable or array element
 *	        containing the environment name value to be set.
 *
 *  LENNAME     is an input integer variable containing the
 *              character length of NAME. If LENNAME is zero,
 *              trailing blanks are removed.
 *
 *  NEW         is an input character variable or array element
 *              containing the new environment value for the
 *              name=value environment pair.
 *
 *  LENNEW      is an input integer variable containing the
 *              character length of NEW. If LENNEW is zero,
 *              trailing blanks are removed.
 *
 *  IOVERWRITE  is an input integer variable containing a zero
 *              or nonzero number. When the value is zero,
 *              a name=value pair with the name value matching
 *              NAME will not be replaced with a new NAME=NEW
 *              pair. A nonzero IOVERWRITE value will replace
 *              the matching name=value pair with NAME=NEW pair.
 *
 *  IERROR      is an output integer varible that will contain
 *              the status:
 *
 *               zero    - environment varible was changed.
 *
 *               nonzero - PXFSETENV was not successful.
 *
 *              PXFSETENV may return any of the following error
 *              values:
 *
 *               EINVAL	If LENNAME < 0 or LENNAME > LEN(NAME) or
 *			LENNEW < 0 or LENNEW > LEN(NEW)
 *
 *               ENOMEM	If PSFSETENV is unable to obtain memory
 *			to copy NAME and NEW to a new name=value
 *			string.
 *
 *
 */

#ifdef _UNICOS
void
PXFSETENV(
#else
void
_PXFSETENV(
#endif
	_fcd	NAME,
	_f_int	*LENNAME,
	_fcd	NEW,
	_f_int	*LENNEW,
	_f_int  *IOTHERWISE,
	_f_int	*IERROR)
{
  int ilenname, slenname, ilennew, slennew;
  char *buf, *cname, *cnew;

  ilenname = *LENNAME;
  slenname = _fcdlen(NAME);
  ilennew = *LENNEW;
  slennew = _fcdlen(NEW);
  *IERROR = 0;

  /* check if the length of the lenname input argument is valid. */
  if (ilenname < 0 || ilenname > slenname ||
      ilennew < 0 || ilennew > slennew)
    *IERROR = EINVAL;
  else {
    if (ilenname == 0) {
      /*
        * If length is zero, user wants trailing blanks stripped.
        * Otherwise, malloc memory and copy the string adding a
        * NULL terminator.
        */

      cname = _fc_acopy(NAME);
      ilenname = slenname;
    } else {

      cname = (char *) malloc (ilenname + 1);
      if (cname != NULL) {
	memcpy(cname, _fcdtocp(NAME), ilenname);
	cname[ilenname] ='\0';
      } else {
	*IERROR = ENOMEM;
	return;
      }

    }

    /* check if NAME already exists in the envrion variable when
       IOTHERWISE is zero. */
    if (*IOTHERWISE == 0 && getenv(cname) != NULL)
      return;

    if (ilennew == 0) {

      /*
        * If length is zero, user wants trailing blanks stripped.
        * Otherwise, malloc memory and copy the string adding a
        * NULL terminator.
        */

      cnew = _fc_acopy(NEW);
      ilennew = slennew;
    } else {
     
      cnew = (char *) malloc(ilennew + 1);
      if (cnew != NULL) {
	memcpy(cnew, _fcdtocp(NEW), ilennew);
	cnew[ilennew] ='\0';
      } else {
	*IERROR = ENOMEM;
	return;
      }

    }

    if (cname == NULL || cnew == NULL)
      *IERROR = ENOMEM;
    else {

      /* concatenate strings to make cname=cnew pair */
      if ((buf = (char *) malloc(ilenname + ilennew + 2)) == NULL) {
	*IERROR = ENOMEM;
	return;
      } else {
	
	/* create 'cname=cnew' pair */
	strcpy(buf, cname);
	strcat(buf, "=\0");
	strcat(buf, cnew);
	free(cname);
	free(cnew);
      }

      if (putenv(buf) != 0)
	*IERROR = ENOMEM;
    }
  }
  return;
}

#ifndef _UNICOS

void
pxfsetenv_(
	char *NAME,
        _f_int *LENNAME,
        char *VALUE,
        _f_int *LENVAL,
	_f_int *IOTHERWISE,
        _f_int *IERROR,
        _f_int namelen,
        _f_int valuelen)
{
	_PXFSETENV( _cptofcd(NAME, namelen), LENNAME,
		    _cptofcd(VALUE, valuelen), LENVAL,
		    IOTHERWISE, IERROR);
	return;
}

#endif



