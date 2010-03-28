/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
/*
  Copyright (c) 2006, 2007. QLogic Corporation.  All rights reserved.

  Unpublished -- rights reserved under the copyright laws of the United
  States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION OR
  DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
  SECRETS OF QLOGIC CORPORATION. USE, DISCLOSURE, OR REPRODUCTION IS
  PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF QLOGIC,
  CORPORATION.

  U.S. Government Restricted Rights:
  The Software is a "commercial item," as that term is defined at 48
  C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
  and "commercial computer software documentation," as such terms are used
  in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
  48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
  End Users acquire the Software with only those rights set forth in the
  accompanying license agreement. QLogic Corporation, 26650 Aliso Viejo 
  Parkway, Aliso Viejo, CA 92656.
*/

/* Copyright (c) 2005, 2006. PathScale, Inc.  All Rights Reserved. */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "stack.h"
#include "utils.h"

#if defined(BUILD_OS_DARWIN)
/* Linux sys/types.h comment says this  belongs to "old compatibility names
 * for C types"--no indication that it varies with machine word size.
 */
typedef unsigned long ulong;
#endif /* defined(BUILD_OS_DARWIN) */

static inline ulong __pmp_distance(ulong a, ulong b)
{
  return a > b ? a - b: b - a;
}
 
static const char *__pmp_pos_below = "below";
static const char *__pmp_pos_above = "above";
 
static inline const char *__pmp_relative (ulong a, ulong b)
{
  return a < b ? __pmp_pos_below : __pmp_pos_above;
}
 
static inline void __pmp_creak (const char *fmt, ...)
  __attribute__((format (printf, 1, 2)));
 
static inline void __pmp_creak (const char *fmt, ...)
{
  char buf[1024];
  va_list ap;
  int nwrit;
 
  va_start(ap, fmt);
  nwrit = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
 
  write(STDERR_FILENO, buf, nwrit);
}
 
void __pmp_segv (int sig, siginfo_t *info, void *arg)
{
  ulong first = ULONG_MAX, bottom, top, nearest = ULONG_MAX, dist = 0;  
  int probable_cause = 0;
  int nmajor = 0;
  int nminor = 0;
  int guard_page = 0;
  const char *pos = NULL;
  char line[1024];
  int dump_map;
  ulong addr;
 
  dump_map = getenv("F90_DUMP_MAP") != NULL;
 
  addr = (ulong) info->si_addr;
 
  __pmp_creak("\n\n**** Segmentation fault!  Fault address: %p\n\n",
              info->si_addr);
 
  FILE *fp = fopen("/proc/self/maps", "r");
 
  if (fp == NULL)
    goto bail;
 
  if (dump_map)
    __pmp_creak("Dump of memory map:\n");
 
  while (fgets(line, sizeof(line), fp)) {
    int major, minor;
    int count;
    char perms[20];
    char name;
    ulong d;
 
    if (dump_map)
      __pmp_creak("  %s", line);
 
    count = sscanf(line, "%lx-%lx %s %*x %d:%d %*d %c",
                   &bottom, &top, perms, &major, &minor, &name);
 
    if (first == ULONG_MAX)
      first = bottom;

    if ((d = __pmp_distance(bottom, addr)) < __pmp_distance(nearest, addr)) {
      dist = d;
      nearest = bottom;
      nmajor = major;
      nminor = minor;
      pos = __pmp_relative(addr, bottom);
    }
 
    if ((d = __pmp_distance(top, addr)) < __pmp_distance(nearest, addr)) {
      dist = d;
      nearest = top;
      nmajor = major;
      nminor = minor;
      pos = __pmp_relative(addr, top);
    }

    if (addr >= bottom && addr < top && strcmp(perms, "---p") == 0 &&
        count == 5) {
      /* A stack guard page is detected as the address being contained
       * within the mapping, the permissions being "---p" and no name. */
      guard_page = 1;
    }
  }
  fclose(fp);
 
  if (dump_map)
    __pmp_creak("\n");
 
  if (addr == 0) {
    __pmp_creak("This is likely to have been caused by either a null "
                "pointer dereference or a general protection fault.\n");
    probable_cause = 1;
  } else {
    __pmp_creak("Fault address is %lu bytes %s the %s valid\n"
                "mapping boundary, which is at %p.\n",
                dist, pos, addr < first ? "first" : "nearest",
                (void *) nearest);
    if (addr < first) {
      __pmp_creak("\nThis may have been caused by a struct access "
                  "through a null pointer.\n");
      probable_cause = 1;
    }
  }
 
  if (!probable_cause && nmajor == 0 && nminor == 0) {
    if (guard_page) {
      __pmp_creak("\nThis is likely to have been caused by a pthread stack "
                  "overflow.\nYou can use the PSC_OMP_STACK_SIZE environment "
                  "variable to specify\nthe size of each pthread stack, "
                  "though this is subject to\nany pthread stack limitations "
                  "of your pthread library.\n");
      probable_cause = 1;
    }
    else if (nearest == bottom || pos == __pmp_pos_below) {
      __pmp_creak("\nThis is likely to have been caused by a stack "
                  "overflow.\n"
                  "Use your shell's ulimit or limit command to see if "
                  "your\nstack size limit is too small.\n");
      probable_cause = 1;
    }
  }
 
  if (!probable_cause && !dump_map) {
    __pmp_creak("\nYou can obtain a view of your program's memory "
                "map at\n"
                "the time of the crash by rerunning with the "
                "F90_DUMP_MAP\n"
                "environment variable set to a non-empty string.\n");
  }
 
bail:
  abort();
}

void __pmp_catch_segv (void)
{
  #define ALT_STACK_SIZE 8192
  void *new_stack = malloc(ALT_STACK_SIZE);
  stack_t ss = {
    ss_sp: new_stack,
    ss_flags: 0,
    ss_size: ALT_STACK_SIZE,
  };
  struct sigaction sa;
 
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = __pmp_segv;
#if defined(BUILD_OS_DARWIN)
  sa.sa_flags = SA_RESETHAND | SA_ONSTACK | SA_SIGINFO | SA_NODEFER;
#else /* defined(BUILD_OS_DARWIN) */
  sa.sa_flags = SA_ONESHOT | SA_ONSTACK | SA_SIGINFO | SA_NOMASK;
#endif /* defined(BUILD_OS_DARWIN) */
 
  if (sigaltstack(&ss, NULL) == -1) {
    perror("sigaltstack");
    exit(1);
  }
 
  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

#define STACK_MIN 65536
 
#define dprint(...) \
        do { \
                if (verbose) { \
                        fprintf(stderr, __VA_ARGS__); \
                } \
        } while (0)

int __pmp_get_stack_size_limit(const char *new_limit, long long *max_stack_ptr,
                               int nthreads)
{
  struct rlimit rl;
  long long page_size, npages, ncpus;
  long long phys_mem;
  long long max_stack;
  int verbose = getenv("PSC_STACK_VERBOSE") != NULL;

  *max_stack_ptr = 0;
 
  if (getrlimit(RLIMIT_STACK, &rl) == -1) {
    __pmp_warning("could not calculate your stack size limit\n");
    return -1;
  }
 
  if (rl.rlim_cur == RLIM_INFINITY) {
    dprint("No stack size limits currently in place\n");
  } else {
    dprint("Stack size limits: %ld current, %ld maximum\n",
      (long) rl.rlim_cur, (long) rl.rlim_max);
  }
 
#if defined(BUILD_OS_DARWIN)
  page_size = get_sysctl_int("hw.pagesize");
  npages = get_sysctl_int("hw.memsize") / page_size;
  ncpus = get_sysctl_int(SYSCTL_NPROCESSORS_ONLN);
#else /* defined(BUILD_OS_DARWIN) */
  page_size = sysconf(_SC_PAGE_SIZE);
  npages = sysconf(_SC_PHYS_PAGES);
  ncpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif /* defined(BUILD_OS_DARWIN) */
  nthreads = (nthreads > 0) ? nthreads : ncpus;
 
  phys_mem = (long long) page_size * npages;
 
  dprint("Physical memory: %lld bytes\n", phys_mem);
  dprint("Number of CPUs: %lld\n", ncpus);
  dprint("Default number of threads per team: %d\n", nthreads);
 
  max_stack = phys_mem - 64LL * 1048576;
 
  if (max_stack > ULONG_MAX) {
    max_stack = ULONG_MAX;
  }
 
  if (phys_mem > 512LL * 1048576) {
    max_stack -= 128LL * 1048576 * ncpus;
  } else {
    max_stack -= (phys_mem >> 3) * ncpus;
  }

  if (nthreads > 1) {
    /* share max_stack over nthreads for OpenMP programs */
    max_stack /= nthreads;
  }

  dprint("Automatic maximum stack size limit: %lld (%lld%% of RAM)\n",
    max_stack, (max_stack * 100) / phys_mem);

  if (new_limit && *new_limit) {
    double max;
    char *end;
 
    max = strtod(new_limit, &end);
 
    if (errno == ERANGE) {
      __pmp_warning("your requested stack size limit of "
                    "\"%s\" is not well-formed\n", new_limit);
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
      __pmp_warning("bad maximum stack size "
                    "limit of %lld (specified as \"%s\")\n",
        max_stack, new_limit);
      return -1;
    }
 
    if (max_stack > ULONG_MAX) {
      __pmp_warning("treating requested stack "
                    "size limit of %lld as no limit\n",
        max_stack);
      max_stack = RLIM_INFINITY;
    }
 
    if (max_stack == RLIM_INFINITY) {
      dprint("You have asked for no stack size limit\n");
    } else {
      dprint("You have asked for a stack size limit of "
        "%lld (%lld%% of RAM)\n",
        max_stack, (max_stack * 100) / phys_mem);
    }
 
    if (max_stack > phys_mem) {
      __pmp_warning("your requested stack "
                    "size limit is %lld%% of physical memory\n",
                    (max_stack * 100) / phys_mem);
    }
  }
  else if (rl.rlim_cur == RLIM_INFINITY || rl.rlim_cur > max_stack) {
    dprint("Will not automatically reduce stack size limit\n");
  }
 
  if (max_stack > rl.rlim_max)
    max_stack = rl.rlim_max;

  *max_stack_ptr = max_stack;

  return 0;
}
