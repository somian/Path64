/*
 * Copyright 2004, 2005, 2006, 2007 PathScale Inc.  All Rights Reserved.
 */
/*
 
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#if defined(BUILD_OS_DARWIN)
# include <sys/types.h>
# include <sys/sysctl.h>
#endif /* defined(BUILD_OS_DARWIN) */

#include "../../clibinc/liberrno.h"

#define STACK_MIN 65536

/* __stack_size_divisor is a back-door mechanism to allow the OpenMP
 * library to influence the stack sizing of the initial thread of a FORTRAN
 * program. See libopenmp/utils.c for the rest of the story. */

int __stack_size_divisor = 1;

int __set_stack_size_limit(char *new_limit)
{
	struct rlimit rl;
	long long page_size, npages, ncpus;
	long long phys_mem;
	long long max_stack;
	int verbose = getenv("PSC_STACK_VERBOSE") != NULL;

	if (new_limit && !*new_limit) {
		if (verbose) _lcomment(FWSTKNOMOD);
		return 0;
	}

	if (getrlimit(RLIMIT_STACK, &rl) == -1) {
		char buf[256];
		*buf = 0;
		strerror_r(errno, buf, sizeof buf);
		_lwarn(FWSTKNOCALC, buf);
		return -1;
	}

	if (verbose) {
	  if (rl.rlim_cur == RLIM_INFINITY) {
		  _lcomment(FWSTKNOLIM);
	  } else {
		  _lcomment(FWSTKLIM, rl.rlim_cur, rl.rlim_max);
	  }
	}

	page_size = sysconf(_SC_PAGE_SIZE);
#if defined(BUILD_OS_DARWIN)
	{
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM;
	npages = 0;
	size_t size = sizeof(npages);
	if (sysctl(mib, 2, &npages, &size, 0, 0)) {
	  perror("sysctl");
	  }
	npages /= page_size;
	mib[1] = HW_NCPU;
	ncpus = 0;
	size = sizeof(ncpus);
        if (sysctl(mib, 2, &ncpus, &size, 0, 0)) {
	  perror("sysctl");
	  ncpus = 1;
	  }
	}
#else /* defined(BUILD_OS_DARWIN) */
	npages = sysconf(_SC_PHYS_PAGES);
	ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif /* defined(BUILD_OS_DARWIN) */

	phys_mem = (long long) page_size * npages;

	if (verbose) _lcomment(FWSTKPHYSMEM, phys_mem, ncpus);

	max_stack = phys_mem - 64LL * 1048576;

	if (max_stack > ULONG_MAX) {
		max_stack = ULONG_MAX;
	}

	if (phys_mem > 512LL * 1048576) {
		max_stack -= 128LL * 1048576 * ncpus;
	} else {
		max_stack -= (phys_mem >> 3) * ncpus;
	}

        if (__stack_size_divisor > 1) {
		if (verbose) _lcomment(FWSTKPHYSSHR, __stack_size_divisor);
                max_stack /= __stack_size_divisor;
	}

	if (verbose) _lcomment(FWSTKAUTOMAX, max_stack,
	  (max_stack * 100) / phys_mem);

	if (new_limit) {
		double max;
		char *end;

		max = strtod(new_limit, &end);

		if (errno == ERANGE || (0 == strchr("kKmMgG%", *end) && *end)) {
			_lwarn(FWSTKILLFORM, new_limit);
			return -1;
		}

		switch (tolower(*end)) {
		case 'k':
			max *= 1024;
			break;
		case 'm':
			max *= 1048576;
			break;
		case 'g':
			max *= 1073741824;
			break;
		case '%':
			max = phys_mem * max / 100;
			break;
		}

		if (*end && strcasecmp(end + 1, "/cpu") == 0) {
			max *= ncpus;
		}

		if (isinf(max)) {
			max_stack = RLIM_INFINITY;
		}
		else if (max < 0) {
			max_stack = phys_mem + max;
		} else {
			max_stack = max;
		}

		if (max_stack != RLIM_INFINITY && max_stack < STACK_MIN) {
			_lwarn(FWSTKBADMAX, max_stack, new_limit);
			return -1;
		}

		if (max_stack > ULONG_MAX) {
			_lwarn(FWSTKUNLIM, max_stack);
			max_stack = RLIM_INFINITY;
		}

		if (verbose) {
		  if (max_stack == RLIM_INFINITY) {
			  _lcomment(FWSTKINFIN);
		  } else {
			  _lcomment(FWSTKLIMIT, max_stack,
			    (max_stack * 100) / phys_mem);
		  }
		}

		if (max_stack > phys_mem) {
			_lwarn(FWSTKVSPHYS, (max_stack * 100) / phys_mem);
		}
	}
	else if (rl.rlim_cur == RLIM_INFINITY || rl.rlim_cur > max_stack) {
		if (verbose) _lcomment(FWSTKNOREDUCE);
		return 0;
	}

	if (max_stack > rl.rlim_max)
	    	max_stack = rl.rlim_max;

	rl.rlim_cur = max_stack;

	if (setrlimit(RLIMIT_STACK, &rl) == -1) {
		char buf[256];
		*buf = 0;
		strerror_r(errno, buf, sizeof buf);
		_lwarn(FWSTKNOCHG, buf);
		return -1;
	}

	if (getrlimit(RLIMIT_STACK, &rl) == -1) {
		return 0;
	}

	if (rl.rlim_cur != max_stack) {
		_lwarn(FWSTKINCORRECT, max_stack, rl.rlim_cur);
	}

	return 0;
}
