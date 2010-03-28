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


/* USMID @(#) libu/ffio/sqbio.h	92.0	10/08/98 14:57:41 */



/*
 *	Structure definitions for flags and fields that are used as private
 *	information to the buffering layer
 */

struct sqb_f
	{
	bitptr	locbuf;		/* scratch space for bit shifting */

	int	bufsiz;		/* size of each buffer, in bits */
	int	nbuf;		/* number of buffers */
	bitptr	_ptr;		/* bit pointer to current spot in buffer */
	struct sqbio *sqbio_base;	/* pointer to beginning of */
				/* structures describing data buffers */
	bitptr	sqb_buf;	/* pointer to base of data buffers */
	struct	sqbio *sqbio_cur;	/* pointer to current structure */
	long	ffci_flags;
	
	};

/* One structure for each data buffer */
struct sqbio
	{
	unsigned int	status:16;	/* status of buffer */
	unsigned int	userdata:1;	/* data in user's area? */
#ifdef _CRAY
	unsigned int 	unused:47;
#else
	unsigned int 	unused:15;
#endif
	int		_cnt;	/* in bits. If reading, amount of data */
				/* left in current buffer. */
				/* If writing, amount of space left in */
				/* current buffer. */
	struct ffsw iostat;	/* status word */
	bitptr	_base;		/* base of buffer */
	struct sqbio *nxt;	/* linked list */
	uint64 _iowritten;	/* amount of data written. In bytes */
	};

/* Defines for the state of each buffer: */
#define EMPTY 0		/* no outstanding I/O and no data in the buffer*/
#define IOACTIVE 1	/* I/O is outstanding */
#define IODATA 2	/* The buffer contains data */

#define BUFA_DEF_BUFSIZ	32768	/* default size of each buffer in bytes */
#define BUFA_DEF_NUMBUF	2	/* default number of buffers */
extern int _sqb_bksp(struct fdinfo *fio, struct ffsw *stat);
extern int _sqb_close(struct fdinfo *fio, struct ffsw *stat);
extern int _sqb_fcntl(struct fdinfo *fio, int cmd, void *arg, struct ffsw *stat);
extern int _sqb_flush(struct fdinfo *fio, struct ffsw *stat);
extern _ffopen_t _sqb_open(const char *name, int flags, mode_t mode,
        struct fdinfo *fio, union spec_u *spec, struct ffsw *stat, long cbits,
        int cblks, struct gl_o_inf *oinf);
extern ssize_t _sqb_read(struct fdinfo *fio, bitptr bufptr, size_t nbytes,
        struct ffsw *retstat, int fulp, int *ubc);
extern _ffseek_t _sqb_seek(struct fdinfo *fio, off_t pos, int whence,
        struct ffsw *stat);
extern _ffseek_t _sqb_sync(struct fdinfo *fio, struct ffsw *stat, int sync);
extern int _sqb_weod(struct fdinfo *fio, struct ffsw *stat);
extern int _sqb_weof(struct fdinfo *fio, struct ffsw *stat);
extern ssize_t _sqb_write(struct fdinfo *fio, bitptr bufptr, size_t nbytes,
        struct ffsw *retstat, int fulp, int *ubc);
