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


/* USMID @(#) libf/auxarr/c1/aux.h	92.0	10/08/98 14:30:10 */
  

	/* This structure describes the _infoblk created by segldr */
	typedef struct {
		unsigned	vers:7,
				unused:25,
				len:32;
		unsigned	infoblk;
		unsigned	checksum;
		unsigned	loadmdy;
		unsigned	loadhms;
		unsigned	loadpid;
		unsigned	loadpvr;
		unsigned	loadosvr;
		unsigned	loadudt;
		unsigned	preset;
		unsigned	basetext:32,
				basedata:32;
		unsigned	textlen:32,
				datalen:32;
		unsigned	bsslen:32,
				zerolen:32;
		unsigned	cdatalen:32,
				lmlen:32;
		unsigned	vmlen:32,
				mmbase:32;
		unsigned	heapinit:32,
				heapinc:32;
		unsigned	stakinit:32,
				stakinc:32;
		unsigned	usxfirst:32,
				usxlast:32;
		unsigned 	mtptr:32,
				compresptr:32;
		unsigned	clockspeed:32,
				environptr:32;
		unsigned	segptr:32,
				unused3:32;
}info;


typedef struct {
	int begcompadr;		/* First compiler address */
	int endcompadr;		/* Last compiler address */
	int ssdadr;		/* Corresponding ssd address */
}trans;

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

#define MSG1 "Could not allocate memory in $auxinit "
#define MSG2 "Failed to allocate space on SDS: "
#define MSG3 "Internal library error in $auxread "
#define MSG4 "sswrite failed: "
#define MSG5 "ssread failed: "
#define MSG6 "Could not allocate memory "
#define MSG7 "Sdsfree() failed: "
#define MSG8 "Bad address given to $sdsfree "
#define MSG9 "Bad address given to $auxread "
