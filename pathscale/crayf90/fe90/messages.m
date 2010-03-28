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


/* USMID:  "\n@(#)5.0_pl/macros/messages.m	5.1	04/29/99 21:22:31\n" */
 

/*****************\
|* MISCELLANEOUS *|
\*****************/


/********************\
|* SIZES AND LIMITS *|
\********************/

/* EXPANDED_MSG_SIZE   This is the maximum length a message can be once all   */
/*                     arguments have been expanded.  The value was chosen    */
/*                     strictly on the basis of "it was large enough".        */
/* FINAL_MSG_SIZE      The maximum total length of a message when all the     */
/*                     parts are added together.			      */
/* MAX_ERR_LIMIT       The default maximum number of messages that will be    */
/*                     issued.  The compiler keeps going after the limit is   */
/*                     exceeded but no more messages appear.                  */
/* MAX_HDR_SIZE        The maximum length of the program/file/line/column     */
/*                     info output with a message.  Again, the value was      */
/*                     chosen because it was "big enough".                    */
/* MAX_MSG             The largest message number allowed.		      */
/* MAX_FE_MSG          The largest message number allocated to the front-end. */
/* MAX_FOLDER_MSG      The largest message number allocated to the folder.    */
/* ORIG_MSG_SIZE       Enforced by psm.  That is, this is the message length  */
/*                     limit used by psm when a new message is entered.       */
/* LAST_MSG_QUEUE_SIZE This is the size of the queue used to hold information */
/*                     about already issued messages.  It prevents duplicate  */
/*                     messages from being issued when reset_lex is called.   */

# define EXPANDED_MSG_SIZE	500
# define FINAL_MSG_SIZE		(MAX_HDR_SIZE + EXPANDED_MSG_SIZE) 
# define LAST_MSG_QUEUE_SIZE	5
# define MAX_ERR_LIMIT		100 
# define MAX_HDR_SIZE		(MAX_FILE_NAME_SIZE + MAX_FILE_NAME_SIZE)
# define MAX_MSG		9999 
# define MAX_FE_MSG		1999 
# define MAX_FOLDER_MSG		2099
# define ORIG_MSG_SIZE		256		


/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/


/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/


/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/
