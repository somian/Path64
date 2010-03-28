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


/* ====================================================================
 * ====================================================================
 *
 * Module: whirl2c_common.c
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  07-Nov-94 - Original Version
 *
 * Description:
 *
 *   See whirl2c_common.h for comments on this file.
 *
 * ====================================================================
 * ====================================================================
 */

#include <ctype.h>
#include "whirl2c_common.h"
#include "PUinfo.h"


/*----------- Various Utility functions -----------*/

void WHIRL2C_parenthesize(TOKEN_BUFFER tokens)
{
   Prepend_Token_Special(tokens, '(');
   Append_Token_Special(tokens, ')');
} /* WHIRL2C_parenthesize */


/*----------- Functions and state for identifier naming -----------*/

const char * 
WHIRL2C_make_valid_c_name(const char *name)
{
   /* If name==NULL, then return NULL;  otherwise, if a valid name,
    * then keep it unaltered; otherwise, construct a valid name
    * in a new Name_Buf by removing invalid characters (never return
    * NULL for this case)
    */
   const char *return_name = name;
   char       *new_name;
   INT         name_idx;
   
   if (name != NULL)
   {
      /* See if we need to construct a new name. First skip valid
       * characters (alphanumeric or '_', and beginning with
       * an alphabetic or '_' character).
       */
      if (isalpha(name[0]) || name[0] == '_')
      {
	 for (name_idx = 1;
	      (isalnum(name[name_idx]) || 
	       name[name_idx] == '_');
	      name_idx++);
      }
      else /* Incorrect first character */
      {
	 /* Skip to a valid first character & construct a new name below */
	 name_idx = 0;
	 while (name[name_idx] != '\0'   &&
		!isalpha(name[name_idx]) && 
		name[name_idx] != '_')
	    name += 1;
	 return_name = name; /* Just in case (name[name_idx] == '\0') */
      }
      
      /* Did we find an invalid character? */
      if (name[name_idx] != '\0')
      {
	 /* Need to construct a new name.  This should be relatively rare */
	 new_name = strcpy(Get_Name_Buf_Slot(strlen(name) + 1), name);
	 return_name = new_name;
	 while (*name != '\0')
	 {
	    if (isalnum(*name) || *name == '_' || *name == '$')
	    {
	       *new_name++ = *name++;
	    }
	    else
	       name++; /* Skip invalid character */
	 }
	 *new_name = '\0';
      }
   }
   return return_name;
} /* WHIRL2C_make_valid_c_name */

