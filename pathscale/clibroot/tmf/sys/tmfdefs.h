/* USMID @(#)10/kern/sys/tmfdefs.h	10.0	02/18/98 14:05:12 */


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


/*
 *   tmfdefs.h
 *
 *   This module is used to define TMF values used by both the TMF driver
 *   and TMF daemon and its children.
 *
 */

#ifndef __SYS_TMFDEFS_H_
#define	__SYS_TMFDEFS_H_


#define	TMF_DMNDEV		0	/* Minor device number of daemon file */
#define	TMF_REQUEST		1	/* Minor device number of request file*/
#define	TMF_FIRST_STREAM	2	/* Minor device number of first	user  */
					/*			tape stream   */

#define	TMF_MAXMINOR		0x1FF
#define	TMF_MINOR(x)		(int)( (x) & TMF_MAXMINOR )


#define	TMF_MAXFILE		47	/* Maximum file name		      */
#define	TMF_MAXVSN		6	/* Maximum VSN length		      */
#define	TMF_MINVSN		1	/* Minimum VSN length		      */
#define	TMF_MAXDEV		8	/* Maximum characters in device name  */
#define	TMF_MAXDEVPATH		100	/* Maximum size of a device path      */
#define	TMF_MAXDGP		8	/* Maximum characters in device group */
#define	TMF_MAXLDR		8	/* Maximum characters in loader name  */
#define	TMF_MAX_NQSID		16	/* Maximum characters in NQS id	      */
#define	TMF_MAXRSRC		8	/* Maximum characters in a TMF        */
					/*			resource      */
#define	TMF_MAXSTM		79	/* Maximum size of a stream path      */
#define	TMF_MINSTREAMS		1	/* Minimum number of streams	      */
#define	TMF_MAXSTREAMS		1000	/* Maximum number of streams	      */
#define	TMF_MINUSERS		1	/* Minimum number of users	      */
#define	TMF_MAXUSERS		1000	/* Maximum number of users	      */

/*
 *   Define string variable lengths.
 */
#define	L_BLKATTR	8		/* Maximum length of a block attribute*/
#define	L_DEVPATH	(TMF_MAXDEVPATH+1)/* Maximum length of a device path  */
#define	L_FILENAME	(TMF_MAXFILE+1)	/* Maximum length of a TMF file	      */
#define	L_LABEL		80		/* Maximum length of a tape label     */
#define	L_MAXDEV	(TMF_MAXDEV+1)	/* Maximum length of a device name    */
#define	L_MAXDGP	(TMF_MAXDGP+1)	/* Maximum length of a group name     */
#define	L_MAXPATH	(MAXPATHLEN+1)	/* Maximum length of a TMF path	      */
#define	L_MAXVSN	(TMF_MAXVSN+2)	/* Maximum length of a volume name    */
#define	L_MAXLDR	(TMF_MAXLDR+1)	/* Maximum length of a media loader   */
#define	L_NQSID		(TMF_MAX_NQSID+1)/* Maximum length of NQS id	      */
#define	L_RECFMT	8		/* Maximum length of record format    */
#define	L_STMPATH	(TMF_MAXSTM+1)	/* Maximum length of a stream path    */


#ifndef NGROUPS_MAX
#define	NGROUPS_MAX	16
#endif

#endif	/* __SYS_TMFDEFS_H_ */

