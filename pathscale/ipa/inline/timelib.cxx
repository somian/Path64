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


#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "timelib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* utility routine */

static double get_cpu()
{
   struct rusage ru;
   double cpu;

   getrusage (RUSAGE_SELF, &ru);
   cpu = (double)ru.ru_utime.tv_sec +
     (double)ru.ru_utime.tv_usec * 1e-6 +
       (double)ru.ru_stime.tv_sec +
	 (double)ru.ru_stime.tv_usec * 1e-6;
   return(cpu);
}

#define MAX_TIMERS 100

static double cumtime[MAX_TIMERS];
static double start_time[MAX_TIMERS];
static int timer_running[MAX_TIMERS];
static int initialized=0;


/* Timer library routines

   start_timer(timer) 
       starts timer number <timer> running. <timer> must be between 1
       and MAX_TIMERS, currently set to 100.

   stop_timer(timer)
       stop the timer and adds its time to the total time for that timer.

   clear_timer(timer)
       sets the time for a timer to 0

   double get_timer_time(timer)
       gets the elapsed time for timer. If the timer is running,
       this does a stop and restart.

*/


static void init_package()
{
   int i;
   if (!initialized) {
      for (i=0; i < MAX_TIMERS; i++) {
	 cumtime[i] = 0.;
	 start_time[i] = 0.;
	 timer_running[i] = 0;
      }
      initialized = 1;
   }
}

void start_timer (int timer)
{
   int timer_index;

   init_package();
   timer_index  = timer-1;
   if (timer_index < 0 || timer_index >=MAX_TIMERS) {
      fprintf(stderr,"### Timer error: %d is not a valid timer value\n",timer);
      return;
   }

   if (timer_running[timer_index]) return;
   timer_running[timer_index] = 1;
   start_time[timer_index] = get_cpu();
   return;
}

void stop_timer (int timer)
{
   int timer_index;

   init_package();
   timer_index  = timer-1;
   if (timer_index < 0 || timer_index >=MAX_TIMERS) {
      fprintf(stderr,"### Timer error: %d is not a valid timer value\n",timer);
      return;
   }

   if (!timer_running[timer_index]) return;
   timer_running[timer_index] = 0;
   cumtime[timer_index] += (get_cpu() - start_time[timer_index]);
   return;
}

void clear_timer(int timer) 
{
   int timer_index;

   init_package();
   timer_index  = timer-1;
   if (timer_index < 0 || timer_index >=MAX_TIMERS) {
      fprintf(stderr,"### Timer error: %d is not a valid timer value\n",timer);
      return;
   }
   timer_running[timer_index] = 0;
   cumtime[timer_index] = 0.0;
}


double get_timer_time (int timer)
{
   int timer_index,running;
   double cputime;
   
   init_package();
   timer_index  = timer-1;
   if (timer_index < 0 || timer_index >=MAX_TIMERS) {
      fprintf(stderr,"### Timer error: %d is not a valid timer value\n",timer);
      return (0.0);
   }
   running = timer_running[timer_index];
   stop_timer(timer);
   cputime = cumtime[timer_index];
   if (running) start_timer(timer);
   return (cputime);
}


#ifdef __cplusplus
}
#endif
