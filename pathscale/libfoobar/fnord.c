/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008. PathScale Inc. All Rights Reserved.
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
#include <errno.h>
#ifdef KEY /* Bug 14161 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif /* KEY Bug 14161 */
#include "qk.h"


void MAIN__(void) __attribute__((weak));

void MAIN__(void)
{
	fprintf(stderr, "Someone linked a Fortran program with no MAIN__!\n");
	exit(1);
}

#ifdef KEY /* Bug 5089 */
extern void __f90_fpe(int sig, siginfo_t *info, void *arg);
#endif /* KEY Bug 5089 */
extern void __f90_segv(int sig, siginfo_t *info, void *arg);
extern int __f90_main(int argc, char **argv, char **arge);
extern void __set_nls_for_compiler(void);
extern int __set_stack_size_limit(char *);
#ifdef KEY /* Bug 7221 */
static int common_init();
#endif /* KEY Bug 7221 */

int main(int argc, char **argv, char **arge) __attribute__((weak));

int main(int argc, char **argv, char **arge)
{
#ifdef KEY /* Bug 7221 */
  /* This is the initialization required for Fortran-generated code when the
   * main program is coded in Fortran. */
  common_init();
  return __f90_main(argc, argv, arge);
}

/*
 * Initialize Fortran runtimes. This is for use when the main program is coded
 * in C, but the C code will also call Fortran-generated procedures.
 */
void
_PSC_ftn_init(int argc, char **argv)
{
  extern void __f90_set_args(int, char **);
  common_init();
  __f90_set_args(argc, argv); /* arge not used */
}

/* Declaring this outside common_init keeps valgrind from reporting it as a
 * leak. Initializing it to this ridiculous value circumvents a linker bug
 * (see KEY bug 13283.) */
static void *new_stack = /* NULL */ (void *) _PSC_ftn_init;

/*
 * This is the initialization required for Fortran-generated code,
 * whether the main program is coded in Fortran or in C.
 */
static int common_init()
{
#endif /* KEY Bug 7221 */

#ifdef KEY /* Bug 14161 */
  int enable_segv_handler = 1;
  char *c = getenv("PSC_ENABLE_SEGV_HANDLER");
  /* If environ variable exists, it rules */
  if (c) {
    enable_segv_handler = strcmp(c, "0"); /* Nonzero enables */
  } else {
    struct rlimit rlp;
    if (0 == getrlimit(RLIMIT_CORE, &rlp) && rlp.rlim_cur) {
      enable_segv_handler = 0; /* Nonzero core limit disables */
    }
  }
#endif /* KEY Bug 14161 */

  if (enable_segv_handler) {

#if defined(BUILD_OS_DARWIN)
	static const size_t stack_size = MINSIGSTKSZ;
#else /* defined(BUILD_OS_DARWIN) */
	static const size_t stack_size = 4096;
#endif /* defined(BUILD_OS_DARWIN) */
	new_stack = malloc(stack_size);
	stack_t ss = {
		ss_sp: new_stack,
		ss_flags: 0,
		ss_size: stack_size,
	};
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = __f90_segv;
#if defined(BUILD_OS_DARWIN)
	sa.sa_flags = SA_RESETHAND | SA_ONSTACK | SA_SIGINFO | SA_NODEFER;
#else /* defined(BUILD_OS_DARWIN) */
	sa.sa_flags = SA_ONESHOT | SA_ONSTACK | SA_SIGINFO | SA_NOMASK;
#endif /* defined(BUILD_OS_DARWIN) */

	/* KEY Bug 11654: call sigaltstack via a wrapper so we can avoid it
	 * in environments where it doesn't work and causes link-time errors.
	 * In those environments, simply refrain from providing our own SIGSEGV
	 * handler (because in case of stack overflow it's not going to work)
	 * and let the user see the normal SIGSEGV behavior. */
	if (_Qk_sigaltstack(&ss, NULL) == -1) {
	        if (errno != ENOSYS) {
		       perror("sigaltstack");
		       exit(1);
		}
	}
	
	else if (sigaction(SIGSEGV, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

#ifdef KEY /* Bug 5089 */
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = __f90_fpe;
#if defined(BUILD_OS_DARWIN)
	sa.sa_flags = SA_RESETHAND | SA_SIGINFO | SA_NODEFER;
#else /* defined(BUILD_OS_DARWIN) */
	sa.sa_flags = SA_ONESHOT | SA_SIGINFO | SA_NOMASK;
#endif /* defined(BUILD_OS_DARWIN) */
	if (sigaction(SIGFPE, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
#endif /* KEY Bug 5089 */

#ifdef KEY /* Bug 14161 */
  } /* enable_segv_handler */
#endif /* KEY Bug 14161 */

	__set_nls_for_compiler();
	__set_stack_size_limit(getenv("PSC_STACK_LIMIT"));

#ifndef KEY /* Bug 7221 */
        return __f90_main(argc, argv, arge);
#endif /* KEY Bug 7221 */
}
