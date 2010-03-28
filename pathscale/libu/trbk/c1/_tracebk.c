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


#pragma ident "@(#) libu/trbk/c1/_tracebk.c	92.1	07/01/99 13:48:28"
#include <stdio.h>
#include <string.h>
#include <cray/stk.h>

#define  MAXARGS	32	/* Max arglist to print */
#define  MAX_ENT_LEN	32	/* Maximum entry point name length + 1  */

/* Define number of parcels in a return-jump instruction */

#if	_MAXVL > 64
#define RJPARCELS	3
#else
#define RJPARCELS	2
#endif

/*
 *	__tracebk	Prints out a traceback with current values
 *			for arguments.
 *
 *	Arguments:
 *		unit	Output stream (e.g., stdout, stderr)
 *		prevptr	Address of previously active frame package
 *		fpptr	Address of currently active frame package
 *		la	Current LA (Limit Address)
 *		depth	Max depth to take traceback (depth == 2 is a
 *			special case; no arguments are printed)
 *		byreg 	If 1, MathLib routine and arguments may be
 *			pass-by-register
 *		status	-1/0/1 = return(errflag)/exit(0)/exit(1)
 *		argflag	0/1 = No/Yes to print argument list
 */

int 
__tracebk(
	FILE			*unit,
	struct stack_frame	*prevptr,
	struct stack_frame	*fpptr,
	short			la,
	short			depth,
	short			byreg,
	short			status,
	short			argflag
)
{
	long		_subrnm();
	long		baseaddr;
	short		lasttrip;
	short		args1, args2, lineno1, lineno2;
	register short	i, j, args, lineno, onedeep;
	long		recursz;
	long		errflag;
	char		callee[MAX_ENT_LEN];
	char		caller[MAX_ENT_LEN];

	errflag	= 1;
	recursz	= 0;
	onedeep	= (depth == 2);

	if ((depth < 1) || (depth > 50))
		depth	= 25;

	if (_subrnm(prevptr, callee, &baseaddr, &args2, &lineno2, la))
		goto FINISHED;

	args	= args2;
	lineno	= lineno2;

	if (_subrnm(fpptr, callee, &baseaddr, &args1, &lineno1, la))
		goto FINISHED;

#ifdef	DEBUG
 	printf("  In __tracebk(_subrnm --> %s)\n", callee);
#endif

	lasttrip	= 0;

	while ((fpptr != NULL) && (--depth > 0)) {
		register short		m1, m2, m3;
		register short		stkofen, segbase, segsize;
		struct stack_frame	*nextfp;

		m1	= (long) fpptr->retn - RJPARCELS;
		m2	= (m1 >> 2);
		m3	= (m1 & 3) + 'a';
		nextfp	= fpptr->prev;

		if (_subrnm(nextfp, caller, &baseaddr, &args2, &lineno2, la))
			goto FINISHED;

		if ((caller[0] == '$') && ((strcmp(caller, "$START$") == 0) ||
		    (strcmp(caller, "$TSKHEAD") == 0)))
			lasttrip	= 1;

		stkofen	= 0;
		segbase	= 0;
		segsize	= 0;

		if (lineno > 0)
			fprintf(unit, " \"%s\" called by \"%s\" (P = 0%o%c, line %d)",
				callee, caller, m2, m3, lineno);
		else
			fprintf(unit, " \"%s\" called by \"%s\" (P = 0%o%c)",
				callee, caller, m2, m3);

		if (onedeep) {
			errflag	= 0;
			putc('\n', unit);
			goto FINISHED;
		} else
			fprintf(unit, " with %d argument%c\n",
				args, (args == 1) ? ' ' : 's');

		/* Print the Arguments  */

		if (argflag && (args > 0) && (args < 657)) {

			if (((m1 = (long) prevptr) < 4) || ((m1 + args) >= la)) {
				fprintf(unit, " Argument list pointer corrupted -- Traceback terminated\n");
				goto FINISHED;
			}

			m2	= (args > MAXARGS) ? MAXARGS : args;

			for (i = 1; i <= m2; ++i) {
				register long	argument, value;
				char		string[20];

				argument	= *(i + prevptr->prevargs);

				fprintf(unit, "%6d)   %022o", i, argument);

				if (m1 = ((argument >= 0) && (argument < la))) {
					value	= *(long *) argument;
					fprintf(unit, " -> (");
				} else {
					value	= argument;
					fprintf(unit, "  =  ");
				}

				j	= _interpret(value, string);

				if (m1 && (j < 3))
					fprintf(unit, "%s", string);
				else
					fprintf(unit, "%022o = %s", value, string);

				if (m1)
					putc(')', unit);

				putc('\n', unit);
			}

			if (args > MAXARGS) {
				fprintf(unit, "    ... Argument print list truncated\n");
				break;
			}

		} else if ((args == 0) && byreg) {
			fprintf(unit, "    MathLib argument(s) may have been passed by register\n");
		}

		byreg	= 0;

		if (stkofen && (segbase > 0) && (segbase < la)) {
			fprintf(unit, "   <New stack segment built for \"%s\" at 0%o, size = %d = 0%o>\n",
				callee, segbase, segsize, segsize);
		}

		if (lasttrip)
			break;

		if ((caller[0] == 'T') && (strcmp(caller, "TSKSTART") == 0)) {
			if ((segbase > 0) && (segbase < la))
				fprintf(unit, "   <MT stack segment built for \"%s\" at 0%o, size = %d = 0%o>\n",
					callee, segbase, segsize, segsize);
			fprintf(unit, " ---- Traceback continuing, but \"TSKSTART\" may be disconnected from caller ----\n");
		}

		(void) strcpy(callee, caller);

		lineno	= lineno1;
		lineno1	= lineno2;
		args	= args1;
		args1	= args2;
		prevptr	= fpptr;
		fpptr	= nextfp;
	} /* while */

	errflag	= 0;

FINISHED:
	if (errflag) {
		if (recursz < 0)
			fprintf(unit, " <<< Tracebk terminated: Too many recursive routines >>>\n");
		else
			fprintf(unit, " <<< Traceback terminated: Address out of range >>>\n");
	}

ENDED:
	if (status < 0)
		return (errflag);
	else
		exit(status);
}
