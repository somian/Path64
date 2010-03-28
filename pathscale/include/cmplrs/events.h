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


/*

	EVENTS.H

*/
		/*
		 * This structure is used to gather
		 * information from an events section
		 * record. Since each record is variable
		 * length and is context dependent based
		 * on it's type, the fields for arg[1-3]
		 * are large.
		 * 
		 * It is not expected that this structure
		 * would be used in a table or linked list
		 * thus size is less of a concern.
		 */
typedef struct {
    __uint64_t	fevnt_arg1;	    /* 	First argument 
					(Zero if no arguments
					for this event)
				    */
    __uint64_t	fevnt_pre_arg1;	    /* 	Raw arg1 value. 
					Zero for leb128 values.
					For other values, same
					as the fevent_arg1 value 
				    */
    __uint64_t	fevnt_arg2;	    /* 	Second argument 
					(Zero if one or 0 arguments).
				    */
    __uint64_t	fevnt_pre_arg2;	    /* 	Raw arg2 value.
					Like fevnt_pre_arg1.
				    */
    __uint64_t	fevnt_arg3;	    /* 	Third argument 
					(zero if less than 3
					arguments).
				    */
    __uint64_t	fevnt_pre_arg3;	    /* 	Raw arg3 value .
					Like fevnt_pre_arg1.
				    */
    __uint64_t	fevnt_base;	    /* 	Section base address?
					Not filled in or used
					by 
					event_find_record() or
					event_get_next_rec(). 
				    */
    __uint32_t	fevnt_type;	    /* 	Record type.
					the EK_*  value of this
					event.
			 	    */
    __uint32_t	fevnt_offset;	    /* 	Section offset.
				       	incremented by 
				       	event_get_next_rec()
				       	by the length of the encoding
				       	of the event described
				       	by this Full_Events
				       	record. 
					(for event_find_record() is
					incremented by the net number
					of events bytes.) 
				    */
    __uint32_t	fevnt_index;	    /* 	text section index, not
					filled in or used by
					event_find_record() or
					event_get_next_rec(). 
				     */
} Full_Events;

	/*******************************************************/
	/*******************************************************/
		/*
		 * Get information from the events section record
		 * pointed to by p_event and return a pointer 
		 * to the next record.
		 */
char *event_get_next_rec(
	char *,			/* pointer into section */
	__uint32_t ,		/* current offset into text section */
	Full_Events *);		/* information from record */

	/*******************************************************/
	/*******************************************************/
		/*
		 * starting at the given point in the section
		 * pointed to by p_event, find the next events
		 * section record of the given type.
		 * 
		 * It is assumed that the current offset coming 
		 * into this routine will be stored in p_full.
		 * 
		 * Returns EK_NULL if type not found.
		 */
__uint32_t event_find_record(
	char *,			/* pointer into the events section */
	Full_Events *,		/* structure for events record info */
	__uint32_t, 		/* event section type */
	char *);		/* end of the current events section */

	/*******************************************************/
	/*******************************************************/
		/*
		 * Return the name of the the kind based on the
		 * kind number. If I don't reccognize the the kind
		 * return the hex string for the number.
		 * 
		 * The buffer for the string is statically allocated
		 * at compile time. If you want to manipulate the string
		 * or keep it around while making multiple calls to
		 * event_kind_string(), you need to copy it to another 
		 * buffer.
		 * 
		 */
char *event_kind_string(__uint32_t );

	/*******************************************************/
	/*******************************************************/
