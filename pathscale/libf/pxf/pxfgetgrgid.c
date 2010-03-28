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


#pragma ident  "@(#) libf/pxf/pxfgetgrgid.c	92.1	06/29/99 11:36:06"


#include <fortran.h>
#include <errno.h>
#include <liberrno.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include "pxfstruct.h"
#include "table.h"

#ifndef _UNICOS
#include <stddef.h>
#endif

/*
 *  PXFGETGRGID  -- get group information using group ID
 *  (section 9.2.1 of Posix 1003.9-1992) 
 *
 *  Fortran:
 *      SUBROUTINE PXFGETGRGID(IGID,JGROUP,IERROR)
 *      INTEGER IGID,JGROUP,IERROR
 *
 *  Where:
 *
 *  IGID   is an input integer variable containing the
 *         group ID, for which group information is
 *         requested.
 *
 *  JGROUP is an output handle of type 'group' created
 *         with PXFSTRUCTCREATE.
 *
 *  IERROR is an output integer variable that will contain
 *          the status:
 *
 *           zero    - group information was retrieved.
 *
 *           nonzero - PXFGETGRGID was not successful.
 *
 *          PXFGETGRGID may return any of the following error
 *          values:
 *
 *           ENOENT If IGID contains an non-existant group
 *                  ID.
 *
 *           ENOMEM If memory needed by PXFGETGRGID could not
 *                  be allocated.
 *
 *           EBADHANDLE If JGROUP is invalid.
 */

#ifdef _UNICOS
void
PXFGETGRGID(
#else
void
_PXFGETGRGID(
#endif
	     _f_int *IGID,
	     _f_int *JGROUP,
	     _f_int *IERROR
)
{
  struct group *groupsrc, *groupdest, grouptemp;
  int i, vectorlen;
  char *str, **srcvector, **destvector;
  struct pxfhandle pxfhand;

  /* NOTE: copying to a temporary group struct until copy has successfully
   * completed. */
  pxfhand = _pxfhandle_table_lookup(&_pxfhandle_table, *JGROUP);
  if (pxfhand.pxfstructptr == NULL || pxfhand.pxftype != PXF_GROUP) {
    *IERROR = EBADHANDLE;
    return;
  }
  groupdest = pxfhand.pxfstructptr;

  if ((groupsrc = getgrgid(*IGID)) != NULL) {

    /* copy all the contents of the struct and the data
      * structures pointed to by the struct since UNICOS uses
      * static data which may change. */
    /* copy group name */
    if ((grouptemp.gr_name =
	 (char *) malloc((strlen(groupsrc->gr_name)+1)*sizeof(char))) == NULL) {
      *IERROR = ENOMEM;
      return;
    }
    strcpy(grouptemp.gr_name, groupsrc->gr_name);

    /* copy group numeric ID */
    grouptemp.gr_gid = groupsrc->gr_gid;

    /* component: gr_passwd (group password) */
    /*
     * Note: PAssword component currently not supported by Posix 1003.9-1992,
     * but all target OSes support this field, so copy for future support.
     */
    grouptemp.gr_passwd =
       (char *) malloc((strlen(groupsrc->gr_passwd)+1)*sizeof(char));
    if (grouptemp.gr_name == NULL) {
      *IERROR = ENOMEM;
      free(grouptemp.gr_name);
      return;
    }
    (void) strcpy(grouptemp.gr_passwd, groupsrc->gr_passwd);

    /* copy vector of user names */
    srcvector = groupsrc->gr_mem;
    for(vectorlen=0; srcvector[vectorlen] != NULL; vectorlen++);

    /* allocate vector of character string pointers for user names.
     * NOTE: Using calloc so the entries are initiall NULL. This is done
     * so PXFSTRUCTFREE knows which entries need to be free'd. */
    if ((grouptemp.gr_mem =
	 (char **) calloc(vectorlen+1,sizeof(char *))) == NULL) {
      free(grouptemp.gr_name);
      *IERROR = ENOMEM;
      return;
    }
    destvector = grouptemp.gr_mem;
    for(i=0; i < vectorlen; i++) {
      if ((destvector[i] =
	   (char *) malloc((strlen(srcvector[i])+1)*sizeof(char))) == NULL) {
	for(i--; i >= 0; i--) free(destvector[i]);
	free(grouptemp.gr_name);
	free(grouptemp.gr_mem);
	*IERROR = ENOMEM;
	return;
      }
      strcpy(destvector[i], srcvector[i]);
    }
    destvector[vectorlen] = NULL; /* NULL terminate vector of strings */

    *IERROR = 0;

    /* free up any memory held by original struct & copy temp struct to */
    free(groupdest->gr_name);
    free(groupdest->gr_passwd);
    if (groupdest->gr_mem != NULL) {
      for(i=0; groupdest->gr_mem[i] != NULL; i++) {
	free(groupdest->gr_mem[i]);
      }
      free(groupdest->gr_mem);
    }

    *groupdest = grouptemp;
  } else {
    *IERROR = ENOENT;
  }

}

#ifndef _UNICOS
void
pxfgetgrgid_(
	     _f_int *IGID,
	     _f_int *JGROUP,
	     _f_int *IERROR
)
{
  _PXFGETGRGID( IGID, JGROUP, IERROR);
}
#endif






