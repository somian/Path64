/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


#include <stdio.h>
#include "gen_util.h"

void Emit_Header (FILE *hfile, 
		  const char *name,
		  const char * const *interface_desc)
{
  int i;

  if (interface_desc) {
    for (i = 0; interface_desc[i] != NULL; ++i) {
      fprintf(hfile, "%s\n", interface_desc[i]);
    }
  }

  fprintf(hfile, "\n#ifndef %s_INCLUDED\n", name);
  fprintf(hfile, "#define %s_INCLUDED\n", name);

  fprintf (hfile, "#ifdef __cplusplus\n"
		  "extern \"C\" {\n"
		  "#endif\n\n");

  /* Pull in appropriate stuff from common/com/defs.h here, so that
   * the header can be used whether defs.h has been included or not.
   */
  /* mUINT32 was not portable to the LP64 abi and thus this hack 
 * which shouldn't exist does.. We should import defs.h or something
 * that would at least allow us to not duplicate defining these values
 */
  fprintf (hfile, "#include <inttypes.h>\n"
		  "#ifndef defs_INCLUDED\n"
		  "#define defs_INCLUDED\n"
		  "typedef signed int INT;\n"
		  "typedef signed int INT32;\n"
		  "typedef signed long long INT64;\n"
		  "typedef signed char mINT8;\n"
		  "typedef signed short mINT16;\n"
		  "typedef signed int mINT32;\n"
		  "typedef signed long long mINT64;\n"
		  "typedef unsigned int UINT;\n"
		  "typedef unsigned int UINT32;\n"
		  "typedef unsigned long long UINT64;\n"
		  "typedef unsigned char mUINT8;\n"
		  "typedef unsigned short mUINT16;\n"
		  "typedef uint64_t mUINT32;\n"
		  "typedef unsigned long long mUINT64;\n"
		  "typedef int BOOL;\n"
		  "typedef unsigned char mBOOL;\n"
		  "#ifndef TRUE\n"
		  "#define TRUE    ((BOOL) 1)\n"
		  "#endif\n"
		  "#ifndef FALSE\n"
		  "#define FALSE   ((BOOL) 0)\n"
		  "#endif\n"
		  "#if (defined(_LANGUAGE_C) || defined(__GNUC__)) && !defined(inline)\n"
		  "#define inline static __inline\n"
		  "#endif\n"
		  "#endif\n\n");
}

void Emit_Footer (FILE *hfile)
{
  fprintf (hfile, "\n#ifdef __cplusplus\n"
		  "}\n"
		  "#endif\n"
		  "#endif\n");    
}

typedef enum {
  DK_MACRO
} DEFINITION_KIND;

typedef struct definition {
  DEFINITION_KIND kind;
  const char *name;
  const char *s;
  struct definition *next;
} DEFINITION;

static DEFINITION *defs;
static DEFINITION *lastdef;

void Define_Macro (const char *name, const char *def)
{
  DEFINITION *newdef = new DEFINITION;
  newdef->kind = DK_MACRO;
  newdef->name = name;
  newdef->s = def;
  newdef->next = NULL;
  if (defs == NULL) {
    defs = newdef;
  } else {
    lastdef->next = newdef;
  }
  lastdef = newdef;
}

void Emit_Definitions (FILE *hfile, const char *prefix)
{
  DEFINITION *def;

  if (defs != NULL) fprintf(hfile, "\n");

  for (def = defs; def != NULL; def = def->next) {
    int c;
    int pos;
    const char *s = def->s;
    pos = fprintf(hfile, "#define %s%s ", prefix, def->name);
    while (pos++ < 40) fputc(' ', hfile);
    fprintf(hfile, "(\"");
    while (c = *s++) {
      if (c == '\\') {
	fprintf(hfile, "\\\\");
      } else if (c < ' ') {
	switch (c) {
	case '\n':
	  fprintf(hfile, "\\n");
	  break;
	case '\t':
	  fprintf(hfile, "\\t");
	  break;
	default:
	  fprintf(hfile, "\\%03o", c);
	  break;
	}
      } else {
	fputc(c, hfile);
      }
    }
    fprintf(hfile, "\")\n");
  }
}
