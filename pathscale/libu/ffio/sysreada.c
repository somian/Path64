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


#pragma ident "@(#) libu/ffio/sysreada.c	92.1	06/29/99 13:16:47"

#include <sys/types.h>
#ifdef	_CRAY
#include <sys/iosw.h>
#endif
#include <errno.h>
#include <ffio.h>
#include "sysio.h"

#ifdef	__mips
extern short _ffaio_initialized; /* defined in syswritea.c */
#endif

/*
 * _sys_reada() calls the system call reada(2), after doing appropriate
 * conversions on the parameters
 */

ssize_t
_sys_reada(struct fdinfo *fio, bitptr bufptr, size_t nbytes, struct ffsw *retstat, int fulp, int *ubc)
	{
	int ret = 0;
	char *buf;
	struct sys_f *sys_info ;

	buf = BPTR2CP(bufptr);
	if ((BPBITOFF(bufptr) & 7) != 0 || *ubc != 0)
		ERETURN(retstat, FDC_ERR_UBC, 0);

	if (nbytes > 0)
		{
		CLRFFSTAT(*retstat);	/* flag async in progress */
#ifdef	__mips
		AIO_INIT();
		retstat->aiocbp.aio_fildes = fio->realfd;
		retstat->aiocbp.aio_nbytes = nbytes;
		retstat->aiocbp.aio_offset = ((struct sys_f *)fio->lyr_info)->curpos;
		retstat->aiocbp.aio_reqprio = 0;
		sys_info = (struct sys_f *)fio->lyr_info ;
                if(sys_info->aio_callback) {
                   extern void _ffio_aio_callback();
		   retstat->aiocbp.aio_sigevent.sigev_notify = SIGEV_CALLBACK;
		   retstat->aiocbp.aio_sigevent.sigev_func  = _ffio_aio_callback ;
		   retstat->aiocbp.aio_sigevent.sigev_value.sival_ptr  = (void *)retstat ;
                }
                else{
		   retstat->aiocbp.aio_sigevent.sigev_notify = SIGEV_NONE;
                }
		retstat->aiocbp.aio_buf = buf;
		retstat->aiocbp.aio_lio_opcode = 0;
		
		ret = aio_read(&retstat->aiocbp);
		if (ret < 0){
			if (errno == EAGAIN){
				/* The requested asynchronous I/O operation */
				/* was not queued due to system resource */
				/* limitations. */
				/* What to do here? I will try switching */
				/* to synchronous. */
				return(XRCALL(fio, readrtn) fio,
					bufptr, nbytes, retstat, fulp, ubc));
			}
			else
				ERETURN(retstat, errno, 0);
		}
		if (((struct sys_f *)fio->lyr_info)->curpos + nbytes > 
			((struct sys_f *)fio->lyr_info)->endpos)
			((struct sys_f *)fio->lyr_info)->curpos =
				((struct sys_f *) fio->lyr_info)->endpos;
		else
			((struct sys_f *)fio->lyr_info)->curpos += nbytes;
		((struct sys_f *)fio->lyr_info)->needpos = 1;
#else
		if (((struct sys_f *)fio->lyr_info)->nointrio)
			ret = reada(fio->realfd, buf, nbytes,
				(struct iosw *)retstat, 0);
		else {
			LOOP_SYSCALL(ret, reada(fio->realfd, buf, nbytes,
				(struct iosw *)retstat, 0));
		}
		if (ret < 0)
			ERETURN(retstat, errno, 0);
#endif
		}
	else	/* nbytes == 0 */
		{
		retstat->sw_flag = 1;
		FFSTAT(*retstat) = FFCNT; /* I/O is done, and other stat */
					/* fields are set. */
		}

	return (ret);
	}
#ifdef	__mips
void
_ffio_aio_callback( struct ffsw *uiostat )
{
        aiocb_t *a[1];
        ssize_t szret;

                        if (uiostat->sw_flag == 0) {
                                szret = aio_return(&(uiostat->aiocbp));
                                if (szret >= 0) {
                                        uiostat->sw_error = 0;
                                        uiostat->sw_count = szret;
                                }
                                else {
                                        uiostat->sw_error = aio_error(&(uiostat->aiocbp));
                                        uiostat->sw_count = 0;
                                        /*ret = -1;*/
                                }
                                uiostat->sw_flag = 1;
                        }
}
#endif	/* __mips */
