/*
 * Copyright 2004, 2005, 2006 PathScale Inc.  All Rights Reserved.
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

#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(BUILD_OS_DARWIN)
typedef unsigned long ulong;
#endif /* defined(BUILD_OS_DARWIN) */

extern void MAIN__(void);

static inline ulong distance(ulong a, ulong b)
{
	return a > b ? a - b: b - a;
}

static const char *pos_below = "below";
static const char *pos_above = "above";

static inline const char *relative(ulong a, ulong b)
{
	return a < b ? pos_below : pos_above;
}

static inline void creak(const char *fmt, ...)
	__attribute__((format (printf, 1, 2)));

static inline void creak(const char *fmt, ...)
{
	char buf[1024];
	va_list ap;
        int nwrit;

	va_start(ap, fmt);
	nwrit = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	write(STDERR_FILENO, buf, nwrit);
}

void __f90_segv(int sig, siginfo_t *info, void *arg)
{
	ulong first = ULONG_MAX, bottom, top, nearest = ULONG_MAX, dist = 0;
	int probable_cause = 0;
	int nmajor, nminor;
	const char *pos;
	char line[1024];
	int dump_map;
	ulong addr;

	dump_map = getenv("F90_DUMP_MAP") != NULL;

	addr = (ulong) info->si_addr;

	creak("\n\n**** Segmentation fault!  Fault address: %p\n\n",
	      info->si_addr);

	FILE *fp = fopen("/proc/self/maps", "r");

	if (fp == NULL)
		goto bail;

	if (dump_map)
		creak("Dump of memory map:\n");

	while (fgets(line, sizeof(line), fp)) {
		int major, minor;
		ulong d;

		if (dump_map)
			creak("  %s", line);

		sscanf(line, "%lx-%lx %*s %*lx %d:%d",
		       &bottom, &top, &major, &minor);

		if (first == ULONG_MAX)
			first = bottom;

		if ((d = distance(bottom, addr)) < distance(nearest, addr)) {
			dist = d;
			nearest = bottom;
			nmajor = major;
			nminor = minor;
			pos = relative(addr, bottom);
		}

		if ((d = distance(top, addr)) < distance(nearest, addr)) {
			dist = d;
			nearest = top;
			nmajor = major;
			nminor = minor;
			pos = relative(addr, top);
		}
	}
	fclose(fp);

	if (dump_map)
		creak("\n");

	if (addr == 0) {
		creak("This is likely to have been caused by either a null "
		      "pointer dereference or a general protection fault.\n");
		probable_cause = 1;
	} else {
		creak("Fault address is %lu bytes %s the %s valid\n"
		      "mapping boundary, which is at %p.\n",
		      dist, pos, addr < first ? "first" : "nearest",
		      (void *) nearest);
		if (addr < first) {
			creak("\nThis may have been caused by a struct access "
			      "through a null pointer.\n");
			probable_cause = 1;
		}
	}

	if (!probable_cause &&
            (nearest == bottom || pos == pos_below) &&
	    nmajor == 0 && nminor == 0) {
		creak("\nThis is likely to have been caused by a stack "
		      "overflow.\n"
		      "Use your shell's ulimit or limit command to see if "
		      "your\nstack size limit is too small.\n");
		probable_cause = 1;
	}

	if (!probable_cause && !dump_map) {
		creak("\nYou can obtain a view of your program's memory "
		      "map at\n"
		      "the time of the crash by rerunning with the "
		      "F90_DUMP_MAP\n"
		      "environment variable set to a non-empty string.\n");
	}

bail:
	abort();
}

#ifdef KEY /* Bug 5089 */

void __f90_fpe(int sig, siginfo_t *info, void *arg)
{
  char *msg = "";
  switch (info->si_code) {
    case FPE_INTDIV: msg = "integer divide by zero";
      break;
    case FPE_INTOVF: msg = "integer overflow";
      break;
    case FPE_FLTDIV: msg = "floating point divide by zero";
      break;
    case FPE_FLTOVF: msg = "floating point overflow";
      break;
    case FPE_FLTUND: msg = "floating point underflow";
      break;
    case FPE_FLTRES: msg = "floating point inexact result";
      break;
    case FPE_FLTINV: msg = "floating point invalid operation";
      break;
    case FPE_FLTSUB: msg = "subscript out of range";
      break;
  }
  creak("Floating point exception signaled at %lx: %s\n",
    (unsigned long) info->si_addr,
    msg);

  /* backtrace_symbols() just isn't very reliable: you have to use -g and
   * -Wl,--export-dynamic, and even then it sometimes misses __MAIN. So I
   * gave up trying to print a stacktrace. */

  abort();
}
#endif /* KEY Bug 5089 */
