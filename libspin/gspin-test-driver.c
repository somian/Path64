/*
  Copyright (C) 2006. PathScale Inc. All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>

#include "gspin-tree.h"
#include "gspin-io.h"

extern gs_void_t gs_dump_file (gs_t t, FILE *f) ;

int
main (int argc, char *argv[])
{
  gs_t p;
  char *file_name, *outfile_name;
  FILE *f;

  if (argc != 2) {
    printf("usage: gspin <spin_file>\n");
    printf("       \"gspin foo.spin\" will dump to \"foo.spin.txt\".\n");
    exit(1);
  }

  file_name = argv[1];
  outfile_name = (char *) malloc(strlen(file_name) + 10);
  sprintf(outfile_name, "%s.txt", file_name);

  f = fopen(outfile_name, "w");
  if (f == NULL) {
    printf("gspin: cannot write to %s\n", outfile_name);
  }

  p = gs_read_file (argv[1]);
  gs_dump_file (p, f);
  // printf ("struct gspin size: %d (bytes)\n", sizeof (struct gspin));

  fclose(f);

  return 0;
}
