/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#ifndef timelib_INCLUDED
#define timelib_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/* 
 * This file contains the interface information for the timer library.
 *
 * All the routines below take an integer argument timer. 
 * Timer must be between 1 and MAX_TIMERS (currently 100). The resolution of
 * the timers is about 10 usec (despite what the system headers files say).
 *
 * start_timer - Starts the specified timer. If the timer was never started before,
 *               this also acts like clear_timer.
 *
 * stop_timer - Stops the specified timer.
 *
 * clear_timer - Sets the accumulated time for the timer to 0.
 *
 * get_timer_time - Returns the total time (system time + user time) as determined by
 *                  getrusage for the specified timer.
 *
 * print_timer (int timer, char *string) - Prints the value of the specified timer 
 *                  marked with the identifier string in a form suitable for use by the
 *                  analysis scripts.
 *
 * fprint_timer (FILE* f, int timer, char* string) - Just like print_timer, but
 *                  prints to a specified file instead of stdout.
 *
 * print_performance (double perf, char *string) - Prints the value of performance
 *                  marked with the identifier string in a form suitable for use by the
 *                  analysis scripts.
 *
 * fprint_performance (FILE* f, double perf, char *string) - Just like
 *                  print_performance, but prints to a specified file
 *                  instead of stdout.
 *
 * The analysis scripts expect timing information in the form 
 * !!!! <label string, can contain anything but ::> :: <time in seconds>
 * and performance information in the form
 * !!!! <label string, can contain anything but :P:> :P: <performance number>
 *
 * The Fortran interface is identical to the C interface, but the Fortran callable
 * versions of print_timer and print_performance have prototypes of:
 * 
 * SUBROUTINE PRINT_TIMER(TIMER,STRING)
 *     INTEGER TIMER
 *     CHARACTER*(*) STRING
 *
 * SUBROUTINE PRINT_PERFORMANCE(PERFORMANCE,STRING)
 *     DOUBLE PRECISION PERFORMANCE
 *     CHARACTER*(*) STRING
 *
 * The string is limited to 128 characters. */

#include <stdio.h>              /* for FILE definition */

   void start_timer (int timer);
   void stop_timer (int timer);
   void clear_timer(int timer) ;
   double get_timer_time (int timer);
   
#ifdef __cplusplus
}
#endif
#endif /* timelib_INCLUDED */
