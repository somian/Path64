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


#pragma ident "@(#) libu/ffio/fssovfl.c	92.1	06/29/99 13:16:47"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ffio.h>
#include "fssio.h"

static void num2str();

#define ERBLD(buf, str)		buf = strlen(buf) + strcpy(buf, str);
#define EWRITE(str)						\
				{				\
				(void)fflush(stdout);		\
				write(2, str, strlen(str));	\
				}

/*
 * _fss_overflow()
 *	Handle fss overflow
 */
int
_fss_overflow(
struct fdinfo	*fio,
struct ffsw	*stat)
{
	int ret;
	int size;
	char *name;
	char er_buf[256], szstr[16], *cp;
	struct mr_f *mr_info;
	struct sds_f *sds_info;
	struct ffc_info_s locinfo;
/*
 *	If overflow is not permitted, return error.
 */
	if (fio->subtype == FSS_OPT_NOVFL)
		ERETURN(stat, FDC_ERR_FSSOVF, 0);

/*
 *	Check to make sure that file is overflowable
 *	If underlying layer is stream, non-transforming,
 *	non-truncating, and can seek, then is OK.
 */
	XRCALL(fio->fioptr, fcntlrtn) fio->fioptr, FC_GETINFO, &locinfo, stat);
	if (!CAN_OVERFLOW(locinfo))
		ERETURN(stat, FDC_ERR_BADOVF, 0);
/*
 *	Issue message to standard error stating overflow happened.
 */
	cp = er_buf;
	if (fio->class == CLASS_SDS) {
		ERBLD(cp, "SDS");
		sds_info = (struct sds_f *)fio->lyr_info;
		name = sds_info->name;
		size = sds_info->sdssize >> 3;
		sds_info->overflowed = YES;
	}
	else if (fio->class == CLASS_MR) {
		ERBLD(cp, "MR");
		mr_info = (struct mr_f *)fio->lyr_info;
		name = mr_info->name;
		size = mr_info->mrsize >> 3;
		mr_info->overflowed = YES;
	}
	else
		ERETURN(stat, FDC_ERR_INTERR, 0);
		
	ERBLD(cp, " overflow at ");
	num2str(szstr, 15, size);
	ERBLD(cp, szstr);
	ERBLD(cp, " bytes on file: ");
	ERBLD(cp, name);
	ERBLD(cp, "\n");
	EWRITE(er_buf);

	return(1);	/* did overflow... */
}


static void
num2str(str, len, num)
char *str;
int len;
int num;
{
	unsigned a, i, ct;
	int pr, d;

	pr = 0;
	ct = 0;
	a = num;
	for (i = 100000000000; i != 1; i /= 10) {
		if ((pr |= (d = a / i)))
			*str++ = d + '0';
		a %= i;
		ct++;
		if (ct >= len) {
			*str++ = '*';
			*str = '\0';
			return;
		}
	}
	*str++ = a + '0';
	*str = '\0';
}
