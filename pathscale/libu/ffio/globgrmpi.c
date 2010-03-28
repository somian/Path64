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


#pragma ident "@(#) libu/ffio/globgrmpi.c	92.2	10/14/99 15:22:06"

#include <fortran.h>
#include "globio.h"

void checkerr(int value);

/*
 * glio_group_mpi
 *
 * Register a group of MPI processes for collective access to global files.
 * This is a collective function.
 * 
 * The MPI processes must all be executing on the same host, 
 */ 
void
glio_group_mpi(MPI_Comm comm)
{

	glio_group_t *gg;
        shmem_group_t shg;
	int groupsz, myrank;
	int ret;
	int irank;
	int myhost, remhost;
	MPI_Status mpis;
	static int rank0pe, rank0pid;

	gg = &_glio_curgroup;

	checkerr( MPI_Comm_size(comm, &groupsz) );
	
	myrank = -1;
	checkerr( MPI_Comm_rank(comm, &myrank) );

	/*
	 * Check that all processes are running on the same host.
	 * Rank 1 through (groupsz-1) send their host IDs to rank 0.  
	 * Rank 0 verifies that all processes are running on the same
	 * system. 
	 */

	myhost = gethostid();
	if (myrank != 0) {
		checkerr( MPI_Send(&myhost, 1, MPI_INT, 0, 0, comm) );
	}

	if (myrank == 0) {
		for (irank=1; irank<groupsz; irank++) {
			checkerr( MPI_Recv(&remhost, 1, MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, comm, &mpis) );
			if (remhost != myhost) {
				fprintf(stderr,"%s:\n\
    glio_group_mpi was called with an MPI communicator associeated with a\n\
    group of processes that are not all executing on the same host.\n\
    The process with rank 0 and the process with rank %d are on different\n\
    hosts.\n", 			GLOBERRMSG, irank);
				abort();
			}
		}
	}
	gg->groupsz = groupsz;
	gg->myrank = myrank;
	gg->grtype = GR_MPI;
	gg->u.MPI.comm = comm;


#if _CRAYT3E
	/*
	 * Check that all processes are in the same application team.
	 * The method is as follows.  Rank 0 puts its PID in "rank0pe".
	 * Then it sends its PID to all the processes in this MPI group.
	 * Finally, each process GETs "rank0pe" from the appropriate PE
	 * number to confirm that it has the expected PID. 
	 */
	rank0pe = _my_pe();
	rank0pid = getpid();

	if (myrank == 0) {
		for (irank=1; irank<groupsz; irank++) {
			checkerr(MPI_Send(&rank0pe,  1, MPI_INT, irank, 1, comm));
			checkerr(MPI_Send(&rank0pid, 1, MPI_INT, irank, 2, comm));
		}
	} else {
		int pe, pid;
		checkerr( MPI_Recv(&pe,  1, MPI_INT, MPI_ANY_SOURCE, 1, comm,
			&mpis) );
		checkerr( MPI_Recv(&pid, 1, MPI_INT, MPI_ANY_SOURCE, 2, comm,
			&mpis) );
		if (pid != _shmem_int_g(&rank0pid, pe)) {
			fprintf(stderr,"%s:\n\
    glio_group_mpi was called for an MPI communicator associated with a\n\
    group of processes that are not members the same application team.\n",
    			GLOBERRMSG);
			abort();
		}
	}
#endif

}

void
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
glio_group_mpi_(
#else
GLIO_GROUP_MPI(
#endif
_f_int *commp)
{
	glio_group_mpi(*commp);
}


/*
 * checkerr
 * 
 * Checks the return value of MPI functions.
 */
void
checkerr(int value)
{
	char errstr[MPI_MAX_ERROR_STRING];
	int errstrlen;
	if (value != MPI_SUCCESS) {
		MPI_Error_string(value, errstr, &errstrlen);
		fprintf(stderr,"%s:\n\
    glio_group_mpi received MPI error %d:\n\
    %s.\n", GLOBERRMSG, value, errstr);
		abort();
	}
}
