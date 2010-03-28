/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pathscale_defs.h"

const char __pathscale_compiler[] __attribute__((weak))
     = PSC_ROOT_PREFIX "/bin/" PSC_NAME_PREFIX "cc";

static char *new_path = NULL;

/*
 * Deallocate the "NLSPATH=x" string added to the environment by
 * __set_nls_for_compiler()
 */
static void
free_new_path(void) {
  if (new_path) {
    free(new_path);
  }
}

/*
 * This function tells the Fortran runtime where to find the message
 * catalogue files it uses to print error messages.
 *
 * We need to know the complete path to the compiler that was used to
 * build MAIN__, e.g. /foo/bar/bin/pathf90.  The compiler should be
 * emitting that information along with MAIN__ as the variable
 * __pathscale_compiler.
 */
void __set_nls_for_compiler(void)
{
	char *old_path = getenv("NLSPATH");
	char *lib_path = NULL;
	char *slash;

	if ((slash = strrchr(__pathscale_compiler, '/')) == NULL) {
		goto bail;
	}

	if (asprintf(&lib_path, "%.*s/../lib/" PSC_FULL_VERSION "/%%N.cat",
		     slash - __pathscale_compiler, __pathscale_compiler) == -1) {
		perror("asprintf");
		lib_path = NULL;
		goto bail;
	}

	if (old_path) {
		if (asprintf(&new_path, "NLSPATH=%s:%s",
			     old_path, lib_path) == -1) {
			goto bail;
		}
	} else {
		if (asprintf(&new_path, "NLSPATH=%s", lib_path) == -1) {
			goto bail;
		}
	}

	putenv(new_path);

bail:
	if (new_path) {
	  atexit(free_new_path);
	}
	free(lib_path);
}
