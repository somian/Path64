/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


#pragma ident "@(#) libu/util/mtcpu.c	92.1	07/07/99 13:18:33"
#include <stdio.h>
#include <stkstat.h>
#include <time.h>

/*
 *	_mtcpu	STOP support routine to handle generating CPU
 *		usage statistics.
 */

extern	void	STKSTAT();

#ifdef	_UNICOS
#define MSWM	stats.st_hhwm		/* Stack hi-water mark */
#define SOVFL	stats.st_ntgrow		/* Number of stack overflows */
#endif

extern	long	*$memhwm;		/* Memory hi-water mark */
extern	long	ZQMAXCPU;		/* Number of available CPUs */
extern	long	_rtstart;		/* RT-clock at program startup */
extern	double	_second();		/* Elapsed CPU time in seconds */

void
_mtcpu()
{
	struct stkstat	stats;		/* Stack statistics struct */
	double	waltime, clks, percent;

	/* Print total clocks run, wallclock time, and % of machine used */

	waltime	= (double) (_rtc() - _rtstart) / (double) CLK_TCK;
	clks	= _second();
	percent	= (waltime > 0.0) ?
			(100.0 * clks / (waltime * (double) ZQMAXCPU)) : 0.0;

	if (waltime >= 0.0) {
		if (percent > 0.05 && percent < 99.9999)
			(void) fprintf(stderr,
			" CPU: %.3fs,  Wallclock: %.3fs,  %.1f%% of %d-CPU Machine\n",
				clks, waltime, percent, ZQMAXCPU);
		else
			(void) fprintf(stderr,
				" CPU: %.3fs,  Wallclock: %.3fs\n", clks,
				waltime);

	}
	else
		/*
		 *  If system reboot after checkpoint, value of wallclock time
	  	 *  may be negative.  Kernel mod only fixes CX/CEA systems.
		 *  Problem can still occur for Cray-2 systems.  Do not give
		 *  bad wallclock value.
		 */
		(void) fprintf(stderr, " CPU: %.3fs\n", clks);

	/* Print memory/stack hi-water marks and number of overflows */

	STKSTAT(&stats);

	(void) fprintf(stderr,
		" Memory HWM: %d, Stack HWM: %d, Stack segment expansions: %d\n",
		$memhwm, MSWM, SOVFL);

	return;
}
