/*
 * Copyright (C) 2008-2009 Advanced Micro Devices, Inc.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 PathScale, LLC.  All Rights Reserved.
 */

/*
 *  Copyright (C) 2006, 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


#include <string.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <ctype.h>
#include "cmplrs/rcodes.h"
#include "opt_actions.h"
#include "options.h"
#include "option_names.h"
#include "option_seen.h"
#include "lang_defs.h"
#include "errors.h"
#include "file_utils.h"
#include "file_names.h"
#include "string_utils.h"
#include "get_options.h"
#include "objects.h"
#include "pathscale_defs.h"
#include "phases.h"
#include "run.h"
#include "profile_type.h" /* for enum PROFILE_TYPE */

/* keep list of previous toggled option names, to give better messages */
typedef struct toggle_name_struct {
	int *address;
	char *name;
} toggle_name;
#define MAX_TOGGLES	50
static toggle_name toggled_names[MAX_TOGGLES];
static int last_toggle_index = 0;
static int inline_on_seen = FALSE;
int inline_t = UNDEFINED;
#ifdef KEY
/* Before front-end: UNDEFINED.  After front-end: TRUE if inliner will be run.
   Bug 11325. */
int run_inline;
int malloc_algorithm = UNDEFINED;
#endif
boolean dashdash_flag = FALSE;
boolean read_stdin = FALSE;
boolean xpg_flag = FALSE;
int default_olevel = 2;
static int default_isa = UNDEFINED;
static int default_proc = UNDEFINED;
int ffast_math_prescan;  // Bug 14302: ffast_math set in option prescan
int instrumentation_invoked = UNDEFINED;
int profile_type = 0;
boolean ftz_crt = FALSE;
#ifndef TARG_MIPS
int isa = UNDEFINED; /* defined from options table */
#endif
int proc = UNDEFINED;
#if defined (TARG_X8664) || defined (TARG_IA32)
static int target_supported_abi = UNDEFINED;
static boolean target_supports_sse2 = FALSE;
static boolean target_prefers_sse3 = FALSE;
static boolean target_supports_3dnow = FALSE;
static boolean target_supports_sse4a = FALSE;
#endif

extern boolean parsing_default_options;
extern boolean drop_option;

static void set_cpu(char *name, m_flag flag_type);
static void add_hugepage_desc(HUGEPAGE_ALLOC, HUGEPAGE_SIZE, int);

#ifdef KEY
void set_memory_model(char *model);
static int get_platform_abi();
#endif

#ifdef TARG_X8664
static void Get_x86_ISA();
static boolean Get_x86_ISA_extensions();
#endif

/* ====================================================================
 *
 * -Ofast targets
 *
 * Given an -Ofast option, tables which map the IP numbers to
 * processors for use in Ofast_Target below.
 *
 * See common/com/MIPS/config_platform.h.
 *
 * PV 378171:  Change this and config.c to use an external table.
 *
 * ====================================================================
 */

int ofast = UNDEFINED;	/* -Ofast toggle -- implicit in Process_Ofast */


static void
add_toggle_name (int *obj, char *name)
{
	int i;
	for (i = 0; i < last_toggle_index; i++) {
		if (obj == toggled_names[i].address) {
			break;
		}
	}
	if (i == last_toggle_index) {
		if (last_toggle_index >= MAX_TOGGLES) {
			internal_error("too many toggle names\n");
		} else {
			last_toggle_index++;
		}
	}
	toggled_names[i].address = obj;
	toggled_names[i].name = string_copy(option_name);
}

static char *
get_toggle_name (int *obj)
{
	int i;
	for (i = 0; i < last_toggle_index; i++) {
		if (obj == toggled_names[i].address) {
			return toggled_names[i].name;
		}
	}
	internal_error("no previously toggled name?");
	return "<unknown>";
}

/* return whether has been toggled yet */
boolean
is_toggled (int obj)
{
	return (obj != UNDEFINED);
}

/* set obj to value; allow many toggles; last toggle is final value */
void
toggle (int *obj, int value)
{
	// Silently drop a default option if it is already toggled on the
	// command line.
	if (parsing_default_options &&
	    is_toggled(*obj)) {
	  drop_option = TRUE;
	  return;
	}

	if (*obj != UNDEFINED && *obj != value) {
		warning ("%s conflicts with %s; using latter value (%s)", 
			get_toggle_name(obj), option_name, option_name);
	}
	*obj = value;
	add_toggle_name(obj, option_name);
}

/* ====================================================================
 *
 * Get_Group_Option_Value
 *
 * Given a group option string, search for the option with the given
 * name.  Return NULL if not found, the option value if found ("" if
 * value is empty).
 *
 * ====================================================================
 */

static char *
Get_Group_Option_Value (
  char *arg,	/* Raw option string */
  char *name,	/* Suboption full name */
  char *abbrev)	/* Suboption abbreviation */
{
  char *endc = arg;
  int n;

  while ( TRUE ) {
    n = strcspn ( arg, ":=" );
    if ( strncasecmp ( arg, abbrev, strlen(abbrev) ) == 0
      && strncasecmp ( arg, name, n ) == 0 )
    {
      endc += n;
      if ( *endc == '=' ) {
	/* Duplicate value lazily: */
	char *result = strdup ( endc+1 );

	* ( result + strcspn ( result, ":=" ) ) = 0;
	return result;
      } else {
	/* No value: */
	return "";
      }
    }
    if ( ( endc = strchr ( arg, ':' ) ) == NULL ) return NULL;
    arg = ++endc;
  }

  /* Shouldn't get here, but ... */
  /* return NULL;  compiler gets better */
}

/* ====================================================================
 *
 * Bool_Group_Value
 *
 * Given a group option value string for a Boolean group value,
 * determine whether it is TRUE or FALSE.
 *
 * ====================================================================
 */

static boolean
Bool_Group_Value ( char *val )
{
  if ( *val == 0 ) {
    /* Empty string is TRUE for group options */
    return TRUE;
  }

  if ( strcasecmp ( val, "OFF" ) == 0
    || strcasecmp ( val, "NO" ) == 0
    || strcasecmp ( val, "FALSE" ) == 0
    || strcasecmp ( val, "0" ) == 0 )
  {
    return FALSE;
  } else {
    return TRUE;
  }
}
#ifdef KEY /* Bug 4210 */

/* ====================================================================
 *
 * Routine to process "-module dirname" and pass "-Jdirname" to Fortran
 * front end
 *
 * ====================================================================
 */
char *f90_module_dir = 0;

void
Process_module ( char *dirname )
{
  if (0 != f90_module_dir)
  {
    error("Only one -module option allowed");
  }
  strcat(
    strcpy(
      f90_module_dir = malloc(sizeof "-J" + strlen(dirname)),
      "-J"),
    dirname);
}
#endif /* KEY Bug 4210 */

/* ====================================================================
 *
 * Routine to manage the implications of -Ofast.
 *
 * Turn on -O3 and -IPA.  Check_Target below will deal with the ABI and
 * ISA implications later.
 *
 * ====================================================================
 */

void
Process_Ofast ( char *ipname )
{
  int flag;
  char *suboption;

  /* -O3: */
  if (!Gen_feedback) {
     O3_flag = TRUE;
     toggle ( &olevel, 3 );
     add_option_seen ( O_O3 );

#ifdef TARG_IA64
     ftz_crt = TRUE;	// flush to zero
#endif

     /* -fno-math-errno */
     toggle ( &fmath_errno, 0);
     add_option_seen (O_fno_math_errno);

     /* -ffast-math */
     toggle ( &ffast_math, 1);
     add_option_seen (O_ffast_math);

     /* -IPA: */
     toggle ( &ipa, TRUE );
     add_option_seen ( O_IPA );

     /* -OPT:Ofast=ipname
      * We will call add_string_option using O_OPT_; if the descriptor
      * for it in OPTIONS changes, this code might require change...
      * Build the "Ofast=ipname" string, then call add_string_option:
      */
     toggle ( &ofast, TRUE );
     suboption = concat_strings ( "Ofast=", ipname );
     flag = add_string_option ( O_OPT_, suboption );
     add_option_seen ( flag );
   } else {
     suboption = concat_strings ( "platform=", ipname );
     flag = add_string_option ( O_TARG_, suboption );
     add_option_seen ( flag );
   }
}

/* ====================================================================
 *
 * Process_Opt_Group
 *
 * We've found a -OPT option group.  Inspect it for -OPT:reorg_common
 * options, and set -split_common and -ivpad accordingly.
 *
 * NOTE: We ignore anything that doesn't match what's expected --
 * the compiler will produce reasonable error messages for junk.
 *
 * ====================================================================
 */

void
Process_Opt_Group ( char *opt_args )
{
  char *optval = NULL;

  if ( debug ) {
    fprintf ( stderr, "Process_Opt_Group: %s\n", opt_args );
  }
  
  /* Go look for -OPT:instrument */
  optval = Get_Group_Option_Value ( opt_args, "instrument", "instr");
  if (optval != NULL) {
     instrumentation_invoked = TRUE;
  }

  /* Go look for -OPT:reorg_common: */
  optval = Get_Group_Option_Value ( opt_args, "reorg_common", "reorg");
  if ( optval != NULL && Bool_Group_Value(optval)) {
#ifndef KEY
    /* If we found it, set -Wl,-split_common,-ivpad: */
    add_option_seen ( O_split_common );
    add_option_seen ( O_ivpad );
#endif
  }

#ifdef KEY
  /* Go look for -OPT:malloc_algorithm */
  optval = Get_Group_Option_Value(opt_args, "malloc_algorithm", "malloc_alg");
  if (optval != NULL &&
      atoi(optval) > 0) {
#ifdef TARG_X8664	// Option available only for x86-64.  Bug 12431.
     malloc_algorithm = atoi(optval);
#else
     warning("ignored -OPT:malloc_algorithm because option not supported on"
	     " this architecture");
#endif
  }
#endif
}

void
Process_Default_Group (char *default_args)
{
  char *s;
  int i;

  if ( debug ) {
    fprintf ( stderr, "Process_Default_Group: %s\n", default_args );
  }

  /* Go look for -DEFAULT:isa=mipsN: */
  s = Get_Group_Option_Value ( default_args, "isa", "isa");
  if (s != NULL && same_string_prefix (s, "mips")) {
	default_isa = atoi(s + strlen("mips"));
  }
  /* Go look for -DEFAULT:opt=[0-3]: */
  s = Get_Group_Option_Value ( default_args, "opt", "opt");
  if (s != NULL) {
	default_olevel = atoi(s);
  }
  /* Go look for -DEFAULT:arith=[0-3]: */
  s = Get_Group_Option_Value ( default_args, "arith", "arith");
  if (s != NULL) {
	i = add_string_option (O_OPT_, concat_strings("IEEE_arith=", s));
	add_option_seen (i);
  }
}

/* ====================================================================
 *
 * Routines to manage the target selection (ABI, ISA, and processor).
 *
 * Make sure that the driver picks up a consistent view of the target
 * selected, based either on user options or on defaults.
 *
 * ====================================================================
 */

/* ====================================================================
 *
 * Process_Targ_Group
 *
 * We've found a -TARG option group.  Inspect it for ABI, ISA, and/or
 * processor specification, and toggle the state appropriately.
 *
 * NOTE: We ignore anything that doesn't match what's expected --
 * the compiler will produce reasonable error messages for junk.
 *
 * ====================================================================
 */

void
Process_Targ_Group ( char *targ_args )
{
  char *cp = targ_args;	/* Skip -TARG: */
  char *cpeq;
  char *ftz;

  if ( debug ) {
    fprintf ( stderr, "Process_Targ_Group: %s\n", targ_args );
  }

  ftz = Get_Group_Option_Value ( targ_args, "flush_to_zero", "flush_to_zero");
  if ( ftz != NULL && Bool_Group_Value(ftz)) {
    /* link in ftz.o */
    ftz_crt = TRUE;
  }

  while ( *cp != 0 ) {
    switch ( *cp ) {
      case '3':
#ifdef TARG_X8664
	if (!strncasecmp(cp, "3dnow=on", 9)) {
	  add_option_seen(O_m3dnow);
	  toggle(&m3dnow, TRUE);
	} else if (!strncasecmp(cp, "3dnow=off", 10)) {
	  add_option_seen(O_mno_3dnow);
	  toggle(&m3dnow, FALSE);
	}
	break;
#endif

      case 'a':
	if ( strncasecmp ( cp, "abi", 3 ) == 0 && *(cp+3) == '=' ) {
#ifdef TARG_MIPS
	  if ( strncasecmp ( cp+4, "n32", 3 ) == 0 ) {
	    add_option_seen ( O_n32 );
	    toggle ( &abi, ABI_N32 );
	  } else if ( strncasecmp ( cp+4, "n64", 3 ) == 0 ) {
	    add_option_seen ( O_64 );
	    toggle ( &abi, ABI_64 );
	  }
#endif
#ifdef TARG_X8664
	  // The driver needs to handle all the -TARG options that it gives to
	  // the back-end, even if these -TARG options are not visible to the
	  // user.  This is because IPA invokes the driver with back-end
	  // options.  Bug 5466.
	  if ( strncasecmp ( cp+4, "n32", 3 ) == 0 ) {
	    add_option_seen ( O_m32 );
	    toggle ( &abi, ABI_N32 );
	  } else if ( strncasecmp ( cp+4, "n64", 3 ) == 0 ) {
	    add_option_seen ( O_m64 );
	    toggle ( &abi, ABI_64 );
	  }
#endif
	}
	break;

#if 0	  /* temporary hack by gbl -- O_WlC no longer exists due to a change in OPTIONS */
      case 'e':
	if ( strncasecmp ( cp, "exc_enable", 10 ) == 0 && *(cp+10) == '=' ) {
  	  int flag;
  	  buffer_t buf;
	  int mask = 0;
	  cp += 11;
    	  while ( *cp != 0 && *cp != ':' ) {
	    switch (*cp) {
	    case 'I': mask |= (1 << 5); break;
	    case 'U': mask |= (1 << 4); break;
	    case 'O': mask |= (1 << 3); break;
	    case 'Z': mask |= (1 << 2); break;
	    case 'D': mask |= (1 << 1); break;
	    case 'V': mask |= (1 << 0); break;
	    }
	    ++cp;
	  }
	  flag = add_string_option(O_WlC, "-defsym,_IEEE_ENABLE_DEFINED=1");
	  add_option_seen (flag);
	  sprintf(buf, "-defsym,_IEEE_ENABLE=%#x", mask);
	  flag = add_string_option(O_WlC, buf);
	  add_option_seen (flag);
	}
	break;
#endif

      case 'i':
	/* We support both isa=mipsn and plain mipsn in group.
	 * Simply move cp to point to value, and fall through to
	 * 'm' case:
	 */
	if ( strncasecmp ( cp, "isa", 3 ) != 0 || *(cp+3) != '=' ) {
	  break;
	} else {
	  cp += 4;
	}
	/* Fall through */

      case 'm':
#ifdef TARG_MIPS
	if ( strncasecmp ( cp, "mips", 4 ) == 0 ) {
	  char c = *(cp+4);
	  if ( '1' <= c && c <= '6' ) {
	    if (c < '6')
	      toggle ( &isa, *(cp+4) - '0' );
	    else { // c == '6'
	      if (*(cp+5) == '\0') // option is mips6
	        toggle ( &isa, ISA_MIPS6 );
	      else // option is mips64
	        toggle ( &isa, ISA_MIPS64 );
	    }
	    switch ( isa ) {
	      case ISA_MIPS1:	add_option_seen ( O_mips1 );
				break;
	      case ISA_MIPS2:	add_option_seen ( O_mips2 );
				break;
	      case ISA_MIPS3:	add_option_seen ( O_mips3 );
				break;
	      case ISA_MIPS4:	add_option_seen ( O_mips4 );
				break;
	      case ISA_MIPS64:   add_option_seen ( O_mips64 );
	      			break;
	      default:		error ( "invalid ISA: %s", cp );
				break;
	    }
	  }
	}
#endif
	break;

      case 'p':
#ifdef KEY
	if (!strncasecmp(cp, "processor=", 10)) {
	  char *target = cp + 10;
	  set_cpu (target, M_ARCH);
	}
#endif
	break;

      case 's':
#ifdef TARG_X8664
	if (!strncasecmp(cp, "sse=on", 7)) {
	  add_option_seen(O_msse);
	  toggle(&sse, TRUE);
	} else if (!strncasecmp(cp, "sse=off", 8)) {
	  add_option_seen(O_mno_sse);
	  toggle(&sse, FALSE);
	} else if (!strncasecmp(cp, "sse2=on", 8)) {
	  add_option_seen(O_msse2);
	  toggle(&sse2, TRUE);
	} else if (!strncasecmp(cp, "sse2=off", 9)) {
	  add_option_seen(O_mno_sse2);
	  toggle(&sse2, FALSE);
	} else if (!strncasecmp(cp, "sse3=on", 8)) {
	  add_option_seen(O_msse3);
	  toggle(&sse3, TRUE);
	} else if (!strncasecmp(cp, "sse3=off", 9)) {
	  add_option_seen(O_mno_sse3);
	  toggle(&sse3, FALSE);
	}else if (!strncasecmp(cp, "sse4a=on", 9)){
          add_option_seen(O_mno_sse4a);
          toggle(&sse4a, TRUE);
        }else if (!strncasecmp(cp, "sse4a=off", 10)){
          add_option_seen(O_mno_sse4a);
          toggle(&sse4a, FALSE);
        }
#endif
	break;
    }

    /* Skip to the next group option: */
    while ( *cp != 0 && *cp != ':' ) ++cp;
    if ( *cp == ':' ) ++cp;
  }
}


/* ====================================================================
 *
 * Check_Target
 *
 * Verify that the target selection is consistent and set defaults.
 *
 * ====================================================================
 */

void
Check_Target ( void )
{
  int opt_id;
  int opt_val;

  if ( debug ) {
    fprintf ( stderr, "Check_Target ABI=%d ISA=%d Processor=%d\n",
	      abi, isa, proc );
  }

#ifdef TARG_X8664
  if (target_cpu == NULL) {
    set_cpu ("auto", M_ARCH);	// Default to auto.
  }

  // Uses ABI to determine ISA.  If ABI isn't set, it guesses and sets the ABI.
  Get_x86_ISA();
#endif

#ifdef TARG_MIPS
  if (target_cpu == NULL) {
    set_cpu ("mips5kf", M_ARCH);	// Default to mips5kf.
  } else if (! strcmp(target_cpu, "auto") ||
	     ! strcmp(target_cpu, "5kf") ||  // Bug 14152
	     ! strcmp(target_cpu, "ice9")) {
    target_cpu = "mips5kf";
  } else if (! strcmp(target_cpu, "twc9")) {
    target_cpu = "twc9a";
  }
#endif

  if (abi == UNDEFINED) {
#ifdef TARG_IA64
	toggle(&abi, ABI_I64);
    	add_option_seen ( O_i64 );
#elif 0
/* #elif TARG_IA32 */
/* O_ia32 is not defined or used anywhere else in the codebase */
	toggle(&abi, ABI_IA32);
    	add_option_seen ( O_ia32 );
#elif TARG_MIPS
	// Change default to -64
	toggle(&abi, ABI_64);
    	add_option_seen ( O_64 );
#elif defined(TARG_X8664) || defined (TARG_IA32)
	// User didn't specify ABI.  Use the ABI supported on host.  Bug 8488.
	if (target_supported_abi == ABI_N32) {
	  abi = ABI_N32;
	} else if (target_supported_abi == ABI_64) {
	  abi = (get_platform_abi() == ABI_N32) ?
		  ABI_N32 : target_supported_abi;
	} else if (target_supported_abi == UNDEFINED) {
	  abi = (get_platform_abi() == ABI_64) ? ABI_64 : ABI_N32;
	} else {
	  internal_error ("illegal target_supported_abi");
	}

	if (abi == ABI_64)
	  add_option_seen (O_m64);
	else
	  add_option_seen (O_m32);
#else
	warning("abi should have been specified by driverwrap");
  	/* If nothing is defined, default to -n32 */
    	toggle ( &abi, ABI_N32 );
    	add_option_seen ( O_n32 );
#endif
  }

#ifdef TARG_X8664
  // ABI must be set.
  if (!Get_x86_ISA_extensions())
    return;	// If error, quit instead of giving confusing error messages.
#endif

  /* Check ABI against ISA: */
  if ( isa != UNDEFINED ) {
    switch ( abi ) {
#ifdef TARG_MIPS_FIXME // FIXME
      case ABI_N32:
	if ( isa < ISA_MIPS3 ) {
	  add_option_seen ( O_mips3 );
	  warning ( "ABI specification %s conflicts with ISA "
		    "specification %s: defaulting ISA to mips3",
		    get_toggle_name (&abi),
		    get_toggle_name (&isa) );
	  option_name = get_option_name ( O_mips3 );
	  isa = UNDEFINED;	/* To avoid another message */
	  toggle ( &isa, ISA_MIPS3 );
	}
	break;

      case ABI_64:
	if ( isa < ISA_MIPS3 ) {
	  /* Default to -mips4 if processor supports it: */
	  if ( proc == UNDEFINED || proc >= PROC_R5K ) {
	    opt_id = O_mips4;
	    opt_val = ISA_MIPS4;
	    add_option_seen ( O_mips4 );
	  } else {
	    opt_id = O_mips3;
	    opt_val = ISA_MIPS3;
	    add_option_seen ( O_mips3 );
	  }
	  warning ( "ABI specification %s conflicts with ISA "
		    "specification %s: defaulting ISA to mips%d",
		    get_toggle_name (&abi),
		    get_toggle_name (&isa),
		    opt_val );
	  option_name = get_option_name ( opt_id );
	  isa = UNDEFINED;	/* To avoid another message */
	  toggle ( &isa, opt_val );
	}
	break;
#endif
    }

  } else {
    /* ISA is undefined, so derive it from ABI and possibly processor: */

    switch ( abi ) {
#ifdef TARG_MIPS
      case ABI_N32:
      case ABI_64:
        if (default_isa == ISA_MIPS3) {
	  opt_val = ISA_MIPS3;
	  opt_id = O_mips3;
	}
	else if (default_isa == ISA_MIPS4) {
	  opt_val = ISA_MIPS4;
	  opt_id = O_mips4;
	}
	else {
	  opt_val = ISA_MIPS64;
	  opt_id = O_mips64;
	}
	toggle ( &isa, opt_val );
	add_option_seen ( opt_id );
	option_name = get_option_name ( opt_id );
	break;
#elif TARG_X8664
      case ABI_N32:
      case ABI_64:
	  opt_val = ISA_X8664;
	  toggle ( &isa, opt_val );
	break;
#endif
      case ABI_I32:
      case ABI_I64:
	opt_val = ISA_IA641;
	toggle ( &isa, opt_val );
	break;
      case ABI_IA32:
	opt_val = ISA_IA32;
	toggle ( &isa, opt_val );
	break;
    }
  }
  if (isa == UNDEFINED) {
	internal_error ("isa should have been defined by now");
  }

  /* Check ABI against processor: */
  if ( proc != UNDEFINED ) {
    switch ( abi ) {
#ifdef TARG_MIPS_FIXME // FIXME
      case ABI_N32:
      case ABI_64:
	if ( proc < PROC_R4K ) {
	  warning ( "ABI specification %s conflicts with processor "
		    "specification %s: defaulting processor to r10000",
		    get_toggle_name (&abi),
		    get_toggle_name (&proc) );
	  option_name = get_option_name ( O_r10000 );
	  proc = UNDEFINED;	/* To avoid another message */
	  add_option_seen ( O_r10000 );
	  toggle ( &proc, PROC_R10K );
	}
	break;
#endif
    }
  }

  /* Check ISA against processor: */
  if ( proc != UNDEFINED ) {
    switch ( isa ) {
#ifdef TARG_MIPS_FIXME // FIXME
      case ISA_MIPS1:
	/* Anything works: */
	break;

      case ISA_MIPS2:
      case ISA_MIPS3:
	if ( proc < PROC_R4K ) {
	  warning ( "ISA specification %s conflicts with processor "
		    "specification %s: defaulting processor to r10000",
		    get_toggle_name (&isa),
		    get_toggle_name (&proc) );
	  add_option_seen ( O_r10000 );
	  proc = UNDEFINED;	/* To avoid another message */
	  option_name = get_option_name ( O_r10000 );
	  toggle ( &proc, PROC_R10K );
	}
	break;

      case ISA_MIPS4:
	if ( proc < PROC_R5K ) {
	  warning ( "ISA specification %s conflicts with processor "
		    "specification %s: defaulting processor to r10000",
		    get_toggle_name (&isa),
		    get_toggle_name (&proc) );
	  add_option_seen ( O_r10000 );
	  proc = UNDEFINED;	/* To avoid another message */
	  option_name = get_option_name ( O_r10000 );
	  toggle ( &proc, PROC_R10K );
	}
	break;
#endif
    }
  }
  else if (default_proc != UNDEFINED) {
	/* set proc if compatible */
	opt_id = 0;
#ifdef TARG_MIPS_FIXME // FIXME
	switch (default_proc) {
	case PROC_R4K:
		if (isa <= ISA_MIPS3) {
			opt_id = O_r4000;
		}
		break;
	case PROC_R5K:
		opt_id = O_r5000;
		break;
	case PROC_R8K:
		opt_id = O_r8000;
		break;
	case PROC_R10K:
		opt_id = O_r10000;
		break;
	}
#endif
	if (abi == ABI_I64 || abi == ABI_IA32) {
		opt_id = 0;	/* no proc for i64, ia32 yet */
	}
	if (opt_id != 0) {
		add_option_seen ( opt_id );
		option_name = get_option_name ( opt_id );
		toggle ( &proc, default_proc);
	}
  }

  if ( debug ) {
    fprintf ( stderr, "Check_Target done; ABI=%d ISA=%d Processor=%d\n",
	      abi, isa, proc );
  }
}

/* ====================================================================
 *
 * Routines to manage inlining choices (the -INLINE group and friends).
 *
 * ====================================================================
 */

/* toggle inline for a normal option (not "=on" or "=off") */

static void
toggle_inline_normal(void)
{
  if (inline_t == UNDEFINED)
    inline_t = TRUE;
}

/* toggle inline for "=on" */

static void
toggle_inline_on(void)
{
  if (inline_t == FALSE) {
    warning ("-noinline or -INLINE:=off has been seen, %s ignored",
	     option_name);
  }
  else {

    inline_t = TRUE;
    inline_on_seen = TRUE;
  }
}

/* toggle inline for "=off" */

static void
toggle_inline_off(void)
{
  if (inline_on_seen == TRUE) {
    warning ("Earlier request for inline processing has been overridden by %s",
	     option_name);
  }
  inline_t = FALSE;
}

void
Process_Profile_Arcs( void )
{
  if (strncmp (option_name, "-fprofile-arcs", 14) == 0)
    add_string_option (O_OPT_, "profile_arcs=true");
}

void
Process_Test_Coverage( void )
{
  if (strncmp (option_name, "-ftest-coverage", 15) == 0)
    add_string_option (O_CG_, "test_coverage=true");
}

/* process -INLINE option */
void
Process_Inline ( void )
{
  int more_symbols = TRUE;
  char *args = option_name+7;

  if (strncmp (option_name, "-noinline", 9) == 0
#ifdef KEY
      || strncmp (option_name, "-fno-inline", 11) == 0
#endif
     )
      toggle_inline_off();
  else if (*args == '\0'
#ifdef KEY
           || strncmp (option_name, "-finline", 8) == 0
#endif
          )
    /* Treat "-INLINE" like "-INLINE:=on" for error messages */
    toggle_inline_on();
  else do {
    char *endc;
    *args = ':';
    if ((endc = strchr(++args, ':')) == NULL)
      more_symbols = FALSE;
    else
      *endc = '\0';
    if (strcasecmp(args, "=off") == 0)
      toggle_inline_off();
    else if (strcasecmp(args, "=on") == 0)
      toggle_inline_on();
    else
      toggle_inline_normal();
    args = endc;
  }
  while (more_symbols);
}

/*
 * Processing -F option: ratfor-related stuff for Fortran, but
 * (obsolete) C code generation option in C++ and unknown for C.
 */
void dash_F_option(void)
{
    if (invoked_lang == L_f77) {
	last_phase=earliest_phase(P_ratfor,last_phase);
    } else if (invoked_lang == L_CC) {
	error("-F is not supported: cannot generate intermediate C code");
    } else {
	parse_error("-F", "unknown flag");
    }
}

/* untoggle the object, so it can be re-toggled later */
void
untoggle (int *obj, int value)
/*ARGSUSED*/
{
  *obj = UNDEFINED;
}

/* change path for particular phase(s), e.g. -Yb,/usr */
static void
change_phase_path (char *arg)
{
	char *dir;
	char *s;
	for (s = arg; s != NULL && *s != NIL && *s != ','; s++)
		;
	if (s == NULL || *s == NIL) {
		parse_error(option_name, "bad syntax for -Y option");
		return;
	}
	dir = s+1;
	if (dir[0] == '~' && (dir[1] == '/' || dir[1] == '\0')) {
	    char *home = getenv("HOME");
	    if (home)
		dir = concat_strings(home, dir+1);
	}
	if (!is_directory(dir))
		parse_error(option_name, "not a directory");
	for (s = arg; *s != ','; s++) {
		/* do separate check so can give better error message */
		if (get_phase(*s) == P_NONE) {
			parse_error(option_name, "bad phase for -Y option");
		} else {
			set_phase_dir(get_phase_mask(get_phase(*s)), dir);
#ifdef KEY
			// Special case wgen because it is affected by -Yf but
			// is not considered a front-end (because it does not
			// take C/C++ front-end flags in OPTIONS).
			if (get_phase(*s) == P_any_fe)
			  set_phase_dir(get_phase_mask(P_wgen), dir);
#endif
		}
	}
}

/* halt after a particular phase, e.g. -Hb */
/* but also process -H and warn its ignored */
static void
change_last_phase (char *s)
{
	phases_t phase;
	if (s == NULL || *s == NIL) {
		warn_ignored("-H");
	} else if ( *(s+1)!=NIL) {
		parse_error(option_name, "bad syntax for -H option");
	} else if ((phase=get_phase(*s)) == P_NONE) {
			parse_error(option_name, "bad phase for -H option");
	} else {
			last_phase=earliest_phase(phase, last_phase);
	}
}

void
save_name (char **obj, char *value)
{
	*obj = string_copy(value);
}

static void
check_output_name (char *name)
{
	if (name == NULL) return;
	if (get_source_kind(name) != S_o && file_exists(name)) {
#ifdef KEY
	  // Allow output to .o because .o is never a source.  SiCortex 9445.
	  if (get_source_kind_from_suffix(get_suffix(name)) != S_o)
#endif
		warning("%s %s will overwrite a file that has a source-file suffix", option_name, name);
	}
}

#ifdef KEY /* bug 4260 */
/* Disallow illegal name following "-convert" */
void
check_convert_name(char *name)
{
	static char *legal_names[] = {
	  "big_endian",
	  "big-endian",
	  "little_endian",
	  "little-endian",
	  "native"
	  };
	for (int i = 0; i < ((sizeof legal_names) / (sizeof *legal_names));
	  i += 1) {
	  if (0 == strcmp(name, legal_names[i])) {
	    return;
	  }
	}
	parse_error(option_name, "bad conversion name");
}
#endif /* KEY bug 4260 */

void
check_dashdash (void)
{
#ifndef KEY	// Silently ignore dashdash options in case pathcc is called as
		// a linker.  Bug 4736.
	if(xpg_flag)
	   dashdash_flag = 1;
	else
	   error("%s not allowed in non XPG4 environment", option_name);
#endif
}

static char *
Get_Binary_Name ( char *name)
{
  char *new;
  int len, i;
  new = string_copy(name);
  len = strlen(new);
  for ( i=0; i<len; i++ ) {
    if (strncmp(&new[i], ".x.Counts", 9) == 0) {
      new[i] = 0;
      break;
    }
  }
  return new;
}
 
void
Process_fbuse ( char *fname )
{
  static boolean is_first_count_file = TRUE;
  Use_feedback = TRUE;
  add_string (count_files, fname);
  if (is_first_count_file && (prof_file == NULL))
    prof_file = Get_Binary_Name(drop_path(fname));
  is_first_count_file = FALSE;
}

void
Process_fb_type ( char*  typename )
{
  char str[10];
  int flag, tmp;
  fb_type = string_copy(typename);
  sprintf(str,"fb_type=%s",fb_type);
  flag = add_string_option (O_OPT_, str);
  add_option_seen(flag);

  sscanf (typename, "%d", &tmp);
  profile_type |= tmp; 
}


void
Process_fb_create ( char *fname )
{
   int flag;
   fb_file = string_copy(fname);

   if (instrumentation_invoked == TRUE) {
     /* instrumentation already specified */
     flag = add_string_option (O_OPT_, "instr_unique_output=on");
   }
   else {
     toggle ( &instrumentation_invoked, TRUE );
     flag = add_string_option (O_OPT_, "instr=on:instr_unique_output=on");
   }
   add_option_seen (flag);
}


void 
Process_fb_phase(char *phase)
{
  char str[10];
  int flag;
  fb_phase = string_copy(phase);
  sprintf(str,"fb_phase=%s",fb_phase);
  flag = add_string_option (O_OPT_, str);
  add_option_seen(flag);
}


void
Process_fb_opt ( char *fname )
{
  opt_file = string_copy(fname);
  toggle ( &instrumentation_invoked, FALSE);
}


void
Process_fbexe ( char *fname )
{
  prof_file = string_copy(fname);
}

void
Process_fb_xdir ( char *fname )
{
  fb_xdir = string_copy(fname);
}

void
Process_fb_cdir ( char *fname )
{
  fb_cdir =  string_copy(fname);
}

#ifndef KEY	// -dsm no longer supported.  Bug 4406.
typedef enum {
  DSM_UNDEFINED,
  DSM_OFF,
  DSM_ON
} DSM_OPTION;

static DSM_OPTION dsm_option=DSM_UNDEFINED;
static DSM_OPTION dsm_clone=DSM_UNDEFINED;
static DSM_OPTION dsm_check=DSM_UNDEFINED;

void
set_dsm_default_options (void)
{
  if (dsm_option==DSM_UNDEFINED) dsm_option=DSM_ON;
  if (dsm_clone==DSM_UNDEFINED && invoked_lang != L_CC) dsm_clone=DSM_ON;
  if (dsm_check==DSM_UNDEFINED) dsm_check=DSM_OFF;
}

void
reset_dsm_default_options (void)
{
  dsm_option=DSM_OFF;
  dsm_clone=DSM_OFF;
  dsm_check=DSM_OFF;
}

void
set_dsm_options (void)
{

  if (dsm_option==DSM_ON) {
    add_option_seen(O_dsm);
  } else {
    reset_dsm_default_options();
    if (option_was_seen(O_dsm))
      set_option_unseen(O_dsm); 
  }

  if (dsm_clone==DSM_ON) 
    add_option_seen(O_dsm_clone);
  else
    if (option_was_seen(O_dsm_clone))
      set_option_unseen(O_dsm_clone); 
  if (dsm_check==DSM_ON) 
    add_option_seen(O_dsm_check);
  else
    if (option_was_seen(O_dsm_check))
      set_option_unseen(O_dsm_check); 
}

/* ====================================================================
 *
 * Process_Mp_Group
 *
 * We've found a -MP option group.  Inspect it for dsm request
 * and toggle the state appropriately.
 *
 * NOTE: We ignore anything that doesn't match what's expected --
 * the compiler will produce reasonable error messages for junk.
 *
 * ====================================================================
 */

void
Process_Mp_Group ( char *mp_args )
{
  char *cp = mp_args;	/* Skip -MP: */

  if ( debug ) {
    fprintf ( stderr, "Process_Mp_Group: %s\n", mp_args );
  }

  while ( *cp != 0 ) {
    switch ( *cp ) {
      case 'd':
	if ( strncasecmp ( cp, "dsm", 3 ) == 0 &&
             (*(cp+3)==':' || *(cp+3)=='\0'))
            set_dsm_default_options();
	else if ( strncasecmp ( cp, "dsm=on", 6 ) == 0 )
            set_dsm_default_options();
	else if ( strncasecmp ( cp, "dsm=off", 7 ) == 0 )
            reset_dsm_default_options();
	else if ( strncasecmp ( cp, "dsm=true", 8 ) == 0 )
            set_dsm_default_options();
	else if ( strncasecmp ( cp, "dsm=false", 9 ) == 0 )
            reset_dsm_default_options();
	else
          parse_error(option_name, "Unknown -MP: option");
	break;
      case 'c':
	if ( strncasecmp ( cp, "clone", 5 ) == 0) {
          if ( *(cp+5) == '=' ) {
	    if ( strncasecmp ( cp+6, "on", 2 ) == 0 )
              dsm_clone=DSM_ON;
	    else if ( strncasecmp ( cp+6, "off", 3 ) == 0 )
              dsm_clone=DSM_OFF;
          } else if ( *(cp+5) == ':' || *(cp+5) == '\0' ) {
              dsm_clone=DSM_ON;
          } else
            parse_error(option_name, "Unknown -MP: option");
	} else if ( strncasecmp ( cp, "check_reshape", 13 ) == 0) {
          if ( *(cp+13) == '=' ) {
	    if ( strncasecmp ( cp+14, "on", 2 ) == 0 ) {
              dsm_check=DSM_ON;
	    } else if ( strncasecmp ( cp+14, "off", 3 ) == 0 ) {
              dsm_check=DSM_OFF;
            }
          } else if ( *(cp+13) == ':' || *(cp+13) == '\0' ) {
              dsm_check=DSM_ON;
          } else
            parse_error(option_name, "Unknown -MP: option");
	}
	else
          parse_error(option_name, "Unknown -MP: option");
	break;
    case 'm':
      if (strncasecmp (cp, "manual=off", 10) == 0) {
        set_option_unseen (O_mp);
        reset_dsm_default_options ();
      }
      else
        parse_error(option_name, "Unknown -MP: option");
      break;
    case 'o':
      if (strncasecmp (cp, "open_mp=off", 11) == 0) {
	 Disable_open_mp = TRUE;
      } else if (strncasecmp (cp, "old_mp=off", 10) == 0) {
	 Disable_old_mp = TRUE;
      } else if ((strncasecmp (cp, "open_mp=on", 10) == 0) ||
		 (strncasecmp (cp, "old_mp=on", 9) == 0)) {
           /* No op; do nothing */
      } else {
	 parse_error(option_name, "Unknown -MP: option");
      }
      break;
    default:
          parse_error(option_name, "Unknown -MP: option");
    }

    /* Skip to the next group option: */
    while ( *cp != 0 && *cp != ':' ) ++cp;
    if ( *cp == ':' ) ++cp;
  }

  if ( debug ) {
    fprintf ( stderr, "Process_Dsm_Group done\n" );
  }
}

void
Process_Mp ( void )
{

  if ( debug ) {
    fprintf ( stderr, "Process_Mp\n" );
  }

  if (!option_was_seen (O_mp)) {
    /* avoid duplicates */
    add_option_seen (O_mp);
  }
  set_dsm_default_options();

  if ( debug ) {
    fprintf ( stderr, "Process_Mp done\n" );
  }
}

void Process_Cray_Mp (void) {

  if (invoked_lang == L_f90) {
    /* this part is now empty (we do the processing differently)
     * but left as a placeholder and error-checker.
     */
  }
  else error ("-cray_mp applicable only to f90");
}
#endif

void
Process_Promp ( void )
{

  if ( debug ) {
    fprintf ( stderr, "Process_Promp\n" );
  }

  /* Invoke -PROMP:=on for f77,f90 -mplist for C, and nothing for
   * other languages.
   */
  if (invoked_lang == L_f77 || invoked_lang == L_f90) {
    add_option_seen ( O_promp );
    add_option_seen(add_string_option(O_FE_, "endloop_marker=1"));
  } else if (invoked_lang == L_cc) {
    /* add_option_seen(O_mplist); */
    add_option_seen ( O_promp );
  }
  if ( debug ) {
    fprintf ( stderr, "Process_Promp done\n" );
  }
}

void
Process_Tenv_Group ( char *opt_args )
{
  if ( debug ) {
    fprintf ( stderr, "Process_TENV_Group: %s\n", opt_args );
  }
  
  /* Go look for -TENV:mcmodel=xxx */
  if (strncmp (opt_args, "mcmodel=", 8) == 0) {
    set_memory_model (opt_args + 8);
  }
}

static int
print_magic_path(const char *base, const char *fname)
{
  int m32 = check_for_saved_option("-m32");
  char *slash;
  char *path;

  if (m32) {
    char *sfx;

    asprintf(&path, "%s/32/%s", base, fname);

    if (file_exists(path))
      goto good;
    
    if (ends_with(base, "/lib64")) {
      asprintf(&path, "%.*s/%s", (int)(strlen(base) - 2), base, fname);

      if (file_exists(path))
	goto good;
    }

    sfx = get_suffix(fname);

    if (sfx != NULL &&	// bug 9049
	(!strcmp(sfx, "a") || !strcmp(sfx, "o") || !strcmp(sfx, "so")))
      goto bad;

    if ((slash = strrchr(path, '/')) && strstr(slash, ".so."))
      goto bad;
  }

  asprintf(&path, "%s/%s", base, fname);

  if (file_exists(path))
    goto good;
  
 bad:
  return 0;

 good:
  puts(path);
  return 1;
}

static int
print_phase_path(phases_t phase, const char *fname)
{
  return print_magic_path(get_phase_dir(phase), fname);
}

static int print_relative_path(const char *s, const char *fname)
{
  char *root_prefix = directory_path(get_executable_dir());
  char *base;

  asprintf(&base, "%s/%s", root_prefix, s);
  return print_magic_path(base, fname);
}

/* Keep this in sync with set_library_paths over in phases.c. */

void
print_file_path (char *fname, int exe)
{
  /* Search for fname in usual places, and print path when found. */
  /* gcc does separate searches for libraries and programs,
   * but that seems redundant as the paths are nearly identical,
   * so try combining into one search.
   */

  if (print_relative_path("lib/" PSC_FULL_VERSION, fname))
    return;

  if (print_phase_path(P_be, fname))
    return;

  if (print_phase_path(P_library, fname))
    return;

  if (print_phase_path(P_gcpp, fname))
    return;

  if (print_phase_path(P_gas, fname))
    return;

  if (print_phase_path(P_alt_library, fname))
    return;

  /* not found, so ask gcc */
  int m32 = check_for_saved_option("-m32");
  char *argv[4];
  argv[0] = "gcc";
  argv[1] = m32 ? "-m32" : "-m64";
  asprintf(&argv[2], "-print-%s-name=%s", exe ? "prog" : "file", fname);
  argv[3] = NULL;
  execvp(argv[0], argv);
  fprintf(stderr, "could not execute %s: %m\n", argv[0]);
  exit(1);
}

void
print_multi_lib ()
{
  char *argv[3];
  argv[0] = "gcc";
  asprintf(&argv[1], "-print-multi-lib");
  argv[2] = NULL;
  execvp(argv[0], argv);
  fprintf(stderr, "could not execute %s: %m\n", argv[0]);
  exit(1);
}

mem_model_t mem_model = M_SMALL;
char *mem_model_name = NULL;

void
set_memory_model(char *model)
{
  if (strcmp(model, "small") == 0) {
    mem_model = M_SMALL;
    mem_model_name = "small";
  }
  else if (strcmp(model, "medium") == 0) {
    mem_model = M_MEDIUM;
    mem_model_name = "medium";
  }
  else if (strcmp(model, "large") == 0) {
    mem_model = M_LARGE;
    mem_model_name = "large";
  }
  else if (strcmp(model, "kernel") == 0) {
    mem_model = M_KERNEL;
    mem_model_name = "kernel";
  } else {
    error("unknown memory model \"%s\"", model);
    mem_model_name = NULL;
  }
}

static struct 
{
  char *cpu_name;
  char *target_name;
  int abi;			// CPUs supporting ABI_64 also support ABI_N32
  boolean supports_sse2;	// TRUE if support SSE2
  boolean prefers_sse3;		// TRUE if target prefers code to use SSE3
  boolean supports_3dnow;       // TRUE if target supports 3dnow
  boolean supports_sse4a;       // TRUE if support SSE4a
} supported_cpu_types[] = {
  { "any_64bit_x86",	"anyx86",	ABI_64,		TRUE,	FALSE, FALSE, FALSE},
  { "any_32bit_x86",	"anyx86",	ABI_N32,	FALSE,	FALSE, FALSE, FALSE},
  { "i386",	"anyx86",		ABI_N32,	FALSE,	FALSE, FALSE, FALSE},
  { "i486",	"anyx86",		ABI_N32,	FALSE,	FALSE, FALSE, FALSE},
  { "i586",	"anyx86",		ABI_N32,	FALSE,	FALSE, FALSE, FALSE},
  { "athlon",	"athlon",		ABI_N32,	FALSE,	FALSE, TRUE, FALSE},
  { "athlon-mp", "athlon",		ABI_N32,	FALSE,	FALSE, TRUE, FALSE},
  { "athlon-xp", "athlon",		ABI_N32,	FALSE,	FALSE, TRUE, FALSE},
  { "athlon64",	"athlon64",		ABI_64,		TRUE,	FALSE, TRUE,  FALSE},
  { "athlon64fx", "opteron",		ABI_64,		TRUE,	FALSE, TRUE,  FALSE},
  { "turion",	"athlon64",		ABI_64,		TRUE,	FALSE, TRUE,  FALSE},
  { "i686",	"pentium4",		ABI_N32,	FALSE,	FALSE, FALSE, FALSE},
  { "ia32",	"pentium4",		ABI_N32,	TRUE,	FALSE, FALSE, FALSE},
  { "k7",	"athlon",		ABI_N32,	FALSE,	FALSE, TRUE,  FALSE},
  { "k8",	"opteron",		ABI_64,		TRUE,	FALSE, TRUE,  FALSE},
  { "opteron",	"opteron",		ABI_64,		TRUE,	FALSE, TRUE,  FALSE},
  { "pentium4",	"pentium4",		ABI_N32,	TRUE,	FALSE, FALSE, FALSE},
  { "xeon",	"xeon",			ABI_N32,	TRUE,	FALSE, FALSE, FALSE},
  { "em64t",	"em64t",		ABI_64,		TRUE,	TRUE,  FALSE, FALSE},
  { "core",	"core",			ABI_64,		TRUE,	TRUE,  FALSE, FALSE},
  { "wolfdale",	"wolfdale",		ABI_64,		TRUE,	TRUE,  FALSE, FALSE},
  { "harpertown", "wolfdale",		ABI_64,		TRUE,	TRUE,  FALSE, FALSE},
  { "barcelona","barcelona",		ABI_64,		TRUE,	TRUE,  TRUE,  TRUE},
  { "shanghai",	"barcelona",		ABI_64,		TRUE,	TRUE,  TRUE,  TRUE},
  { "istanbul",	"barcelona",		ABI_64,		TRUE,	TRUE,  TRUE,  TRUE},
  { "nehalem",	"wolfdale",		ABI_64,		TRUE,	TRUE,  FALSE, FALSE},
  { NULL,	NULL, },
};
  
char *target_cpu = NULL;

#if defined(BUILD_OS_DARWIN)
#include <sys/types.h>
#include <sys/sysctl.h>

/* Fetch an integer-valued sysctl parameter
 * name		name of parameter in sysctl database
 * return	result, or ~0 if not found
 */
static long int
get_sysctl_int(char *name) {
  long int old;
  size_t oldlen = sizeof(old);
  if (sysctlbyname(name, &old, &oldlen, 0, 0)) {
    perror("sysctlbyname");
    return (long int) ~0UL;
    }
  return old;
  }

/* Fetch a string-valued sysctl parameter
 * name		name of parameter in sysctl database
 * value	buffer in which to store value
 * value_nbytes	size of buffer
 * return	pointer to input buffer, or empty string for error
 */
static char *
get_sysctl_str(char *name, char *value, size_t value_nbytes) {
  if (sysctlbyname(name, value, &value_nbytes, 0, 0)) {
    /* perror("sysctlbyname"); */
    *value = 0;
    }
  return value;
  }
#endif /* ! defined(BUILD_OS_DARWIN) */

// Get the platform's default ABI.
static int
get_platform_abi()
{
#if defined(BUILD_OS_DARWIN)
  /* "hw.machine" says "i386" for both 32 and 64 bit machines.
   * "machdep.cpu.extfeatures" says "EM64T" on 64 bit machine. Not as general
   * as one would like--what about AMD someday?--but good enough for now. */
  char u[256];
# define SYSCTL_MACHDEP_CPU_EXTFEATURES "machdep.cpu.extfeatures"
  char *tmp = get_sysctl_str(SYSCTL_MACHDEP_CPU_EXTFEATURES, u, sizeof u);
  return (tmp && strstr(tmp, "EM64T")) ? ABI_64 : ABI_N32;
#else /* defined(BUILD_OS_DARWIN) */
  struct utsname u;

  // Note:  One can determine if it is a 64-bit x86 CPU by checking for the
  // "lm" bit in "flags" in /proc/cpuinfo.  Maybe this can be used instead of
  // uname.
  uname(&u);
  if (!strcmp(u.machine, "x86_64"))
    return ABI_64;
  return ABI_N32;
#endif /* defined(BUILD_OS_DARWIN) */
}

// Return the numeric value after ':' in a line in /proc/cpuinfo.
static int
get_num_after_colon (char *str)
{
  char *p;
  int num;

  p = strchr(str, ':');
  if (p == NULL) {
    error ("cannot parse /proc/cpuinfo: missing colon");
  }
  p++;
  if (sscanf(p, "%d", &num) == 0) {
    error ("cannot parse /proc/cpuinfo: missing number after colon");
  }
  return num;
}


// Return the CPU target name to default to as a last resort.
static char *
get_default_cpu_name (char *msg)
{
  char *cpu_name = NULL;
  char *abi_name = NULL;

  if (get_platform_abi() == ABI_64) {
    cpu_name = "anyx86";
    abi_name = "64-bit";
  } else {
    cpu_name = "anyx86";
    abi_name = "32-bit";
  }

  // NULL means not to warn.
  if (msg != NULL)
    warning("%s, defaulting to basic %s x86.", msg, abi_name);

  return cpu_name;
}

// Get CPU name from /proc/cpuinfo.
char *
get_auto_cpu_name ()
{
#if defined(BUILD_OS_DARWIN)
  /* The Linux strategy (fetch the cpu name and look in a table to find
   * the target characteristics we're interested in) may not be right for
   * the Mac, where the sysctl database does not provide a name like "core"
   * or "em64t", but instead provides a brand string containing an Intel
   * Processor number like "T2400", as well as additional database entries for
   * processor family and model numbers. For now, I'll just hardwire the Mac
   * to "xeon" (for 32 bits) and "core" (for 64) and let the Linux strategy
   * take over from there.
   *
   * Typing "sysctl -A on the command line shows what info is available:
   * hw.ncpu: 2
   * hw.byteorder: 1234
   * hw.memsize: 1073741824
   * hw.activecpu: 2
   * hw.physicalcpu: 2
   * hw.physicalcpu_max: 2
   * hw.logicalcpu: 2
   * hw.logicalcpu_max: 2
   * hw.cputype: 7
   * hw.cpusubtype: 4hw.cpufamily: 1943433984
   * hw.cacheconfig: 2 1 2 0 0 0 0 0 0 0
   * hw.pagesize: 4096
   * hw.busfrequency: 664000000
   * hw.busfrequency_min: 664000000
   * hw.busfrequency_max: 664000000hw.cpufrequency: 1830000000
   * hw.cpufrequency_min: 1830000000
   * hw.cpufrequency_max: 1830000000
   * hw.cachelinesize: 64hw.l1icachesize: 32768
   * hw.l1dcachesize: 32768
   * hw.l2cachesize: 2097152
   * hw.tbfrequency: 1000000000
   * hw.optional.floatingpoint: 1
   * hw.optional.mmx: 1
   * hw.optional.sse: 1
   * hw.optional.sse2: 1
   * hw.optional.sse3: 1
   * machdep.cpu.vendor: GenuineIntel
   * machdep.cpu.brand_string: Genuine Intel(R) CPU           T2400  @ 1.83GHz
   * machdep.cpu.model_string: Unknown Intel P6 Family
   * machdep.cpu.family: 6
   * machdep.cpu.model: 14
   * machdep.cpu.extmodel: 0
   * machdep.cpu.extfamily: 0
   * machdep.cpu.feature_bits: -1075184641 49577
   * machdep.cpu.extfeature_bits: 1048576 0machdep.cpu.stepping: 8
   * machdep.cpu.signature: 1768
   * machdep.cpu.brand: 0
   * machdep.cpu.features: FPU VME DE PSE TSC MSR PAE MCE CX8 APIC SEP MTRR PGE MCA CMOV PAT CLFSH DS ACPI MMX FXSR SSE SSE2 SS HTT TM SSE3 MON VMX EST TM2 TPR
   * machdep.cpu.extfeatures: XD
   * machdep.cpu.logical_per_package: 2
   * machdep.cpu.cores_per_package: 2
   */
  char buf[256];

# define MACHDEP_CPU_VENDOR		"machdep.cpu.vendor"
# define MACHDEP_CPU_BRAND_STRING	"machdep.cpu.brand_string"

  /* Demonstrate how to fetch vendor name */
  char *vendor = get_sysctl_str(MACHDEP_CPU_VENDOR, buf, sizeof buf);
  boolean amd = !!strstr(vendor, "AMD");
  boolean intel = !!strstr(vendor, "GenuineIntel");

  /* Demonstrate how to fetch brand string */
  char *brand_string = get_sysctl_str(MACHDEP_CPU_BRAND_STRING, buf,
    sizeof buf);

  char *cpu_name = (get_platform_abi() == ABI_N32) ? "xeon" : "core";
#else /* defined(BUILD_OS_DARWIN) */
  FILE *f;
  char buf[256];
  char *cpu_name = NULL;
  char *cpu_name_64bit = NULL;		// cpu_name of 64-bit version of CPU
  int cpu_family = -1;			// CPU family number
  int model = -1;			// CPU model number
  boolean amd = FALSE;			// AMD CPU
  boolean intel = FALSE;		// Intel CPU

  f = fopen("/proc/cpuinfo", "r");
  if (f == NULL) {
    return get_default_cpu_name("cannot read /proc/cpuinfo");
  }

  // Parse /proc/cpuinfo.
  while (fgets(buf, 256, f) != NULL) {
    // vendor_id
    if (!strncmp("vendor_id", buf, 9)) {
      if (strstr(buf, "AuthenticAMD")) {
	amd = TRUE;
      } else if (strstr(buf, "GenuineIntel")) {
	intel = TRUE;
      } else {
	return get_default_cpu_name("unsupported vendor_id in /proc/cpuinfo");
      }

    // cpu family
    } else if (!strncmp("cpu family", buf, 10)) {
      cpu_family = get_num_after_colon(buf);

    // model
    } else if (!strncmp("model\t", buf, 6) ||
	       !strncmp("model   ", buf, 8)) {
      model = get_num_after_colon(buf);

    // model name
    } else if (!strncmp("model name", buf, 10)) {
      if (strstr(buf, "Opteron")) {		// AMD
	cpu_name = "opteron";
      } else if (strstr(buf, "Athlon")) {
	if (strstr(buf, "64"))
	  cpu_name = "athlon64";
	else if (strstr(buf, "MP"))
	  cpu_name = "athlon-mp";
	else
	  cpu_name = "athlon";
      } else if (strstr(buf, "Turion")) {
	cpu_name = "turion";
      } else if (strstr(buf, "Pentium") &&	// Intel
		 strstr(buf, "4")) {
	cpu_name = "pentium4";
	// Some Pentium 4 have 64-bit support.  Bug 14820.
	cpu_name_64bit = "em64t";
      } else if (strstr(buf, "Xeon")) {
	cpu_name = "xeon";
	cpu_name_64bit = "em64t";
      }
    }
  }

  fclose(f);

  // If /proc/cpuinfo doesn't have a supported model name, try to derive one
  // from the family and model numbers.  If that fails, fall back to a default.
  // Bug 5785.
  if (cpu_name == NULL ||
      // need to differentiate Core-based Xeon's
      !strcmp(cpu_name, "xeon")) {
    char *abi_name;
    if (intel == TRUE) {
      switch (cpu_family) {
	case 3:			// 386
	case 4:			// 486
	case 5:			// Intel P5, P54C, P55C, P24T
	    return "i386";

	case 6:			// P6, Core, ...
	  if (model == 7 ||	// Harpertown	bug 14685
	      model == 23 ||	// Wolfdale
	      model == 26)	// Nehalem
	    return "wolfdale";

	  if (model >= 15)
	    return "core";

	  // Treat the rest of the P6 family as generic x86 since we don't
	  // optimize for them.
	  return "i386";

	case 15:		// P4
	  cpu_name = "xeon";
	  cpu_name_64bit = "em64t";
	  break;
      }

    } else if (amd == TRUE) {
      switch (cpu_family) {
	case 4:			// 5x86
	case 5:			// K5, K6
	case 6:			// K7
	  return "athlon";
	case 15:		// Opteron, Athlon64
	  return "opteron";
        case 16: 
	  if (model == 4)
	    return "shanghai";	// Shanghai
	  else if (model == 8)
	    return "istanbul";	// Istanbul
	  return "barcelona";	// Barcelona
      }
    }

    if (cpu_name == NULL) {
      return get_default_cpu_name("cannot deduce a supported CPU name"
				  " from /proc/cpuinfo");
    }
   // bug 14625 : barcelona has its model_name (thus cpu_name) as 
   // "opteron". We consider it as a barcelona machine as long as
   // cpu_family is 16
  }else if (!strcmp(cpu_name, "opteron") &&
             cpu_family == 16)
        return "barcelona";

  if (cpu_name == NULL) {
    error("cpu_name NULL");
    return NULL;
  }

  // If cpuinfo doesn't say if CPU is 32 or 64-bit, ask the OS.
  if (cpu_name_64bit != NULL) {
    if (get_platform_abi() == ABI_64) {
      cpu_name = cpu_name_64bit;
    }
  }
#endif /* defined(BUILD_OS_DARWIN) */

  return cpu_name;
}

// Find the target name from the CPU name.
static void
set_cpu(char *name, m_flag flag_type)
{
  // If parsing the default options, don't change the target cpu if it is
  // already set.
  if (parsing_default_options &&
      target_cpu != NULL) {
    drop_option = TRUE;
    return;
  }

  // Warn if conflicting CPU targets are specified.
  // XXX We are not compatible with gcc here, which assigns different meanings to the
  // -march, -mtune and -mcpu flags.  We treat them as synonyms, which we should not.
  if (target_cpu != NULL &&
      strcmp(target_cpu, name)) {
    warning("CPU target %s conflicts with %s; using latter (%s)",
	    get_toggle_name((int*)&target_cpu), name, name);
    // Reset target_cpu so that the driver will complain if a new value can't
    // be determined.
    target_cpu = NULL;
  }
  target_cpu = name;
  add_toggle_name((int*)&target_cpu, name);
}

#ifdef TARG_MIPS
// Set ABI based on -mabi=<abi_name>
static void
set_abi(char * abi_name)
{
  if (!strcmp (abi_name, "n32"))
  {
    toggle (&abi, ABI_N32);
    add_option_seen ( O_n32 );
  }
  else if (!strcmp (abi_name, "64"))
  {
    toggle (&abi, ABI_64);
    add_option_seen ( O_64 );
  }
  else error ("unknown ABI \"%s\"", abi_name);
}
#endif

#ifdef TARG_X8664
static void
Get_x86_ISA ()
{
  char *name = NULL;

  // Get a more specific cpu name.
  if (!strcmp(target_cpu, "auto")) {		// auto
    target_cpu = get_auto_cpu_name();		// may return anyx86
    if (target_cpu == NULL)
      return;
  }
  if (!strcmp(target_cpu, "anyx86")) {		// anyx86
    // Need ABI to select any_32bit_x86 or any_64bit_x86 ISA.
    if (abi == UNDEFINED) {
      if (get_platform_abi() == ABI_64) {
	abi = ABI_64;
	add_option_seen(O_m64);
      } else {
	abi = ABI_N32;
	add_option_seen(O_m32);
      }
    }
    switch (abi) {
      case ABI_N32:	name = "any_32bit_x86"; break;
      case ABI_64:	name = "any_64bit_x86"; break;
      default:		internal_error("illegal ABI");
    }
  } else
    name = target_cpu;

  for (int i = 0; supported_cpu_types[i].cpu_name; i++) {
    if (strcmp(name, supported_cpu_types[i].cpu_name) == 0) {
      target_cpu = supported_cpu_types[i].target_name;
      target_supported_abi = supported_cpu_types[i].abi;
      target_supports_sse2 = supported_cpu_types[i].supports_sse2;
      target_prefers_sse3 = supported_cpu_types[i].prefers_sse3;
      target_supports_3dnow = supported_cpu_types[i].supports_3dnow;
      target_supports_sse4a = supported_cpu_types[i].supports_sse4a;
      break;
    }
  }

  if (target_cpu == NULL) {
    error("unknown CPU type \"%s\"", name);
  }
}

// Return TRUE if there is no error, else return FALSE.
static boolean
Get_x86_ISA_extensions ()
{
  // Quit if the user requests an ISA extension that is not available on the
  // target processor.  Add extensions as necessary.  Bug 9692.
  if (sse2 == TRUE &&
      !target_supports_sse2) {
    error("Target processor does not support SSE2.");
    return FALSE;
  }

  if (m3dnow == TRUE &&
      !target_supports_3dnow) {
    error("Target processor does not support 3DNow.");
    return FALSE;
  }

  if (sse4a == TRUE && //todo: sse4a may require sse2
      !target_supports_sse4a) {
    error("Target processor does not support SSE4a.");
    return FALSE;
  }

  if (abi == UNDEFINED) {
    internal_error("Get_x86_ISA_extensions: ABI undefined\n");
    return FALSE;
  }

  // Assume that all targets support SSE.
  if (sse == UNDEFINED)
    sse = TRUE;

  // Use SSE2 on systems that support it.
  if (sse2 == UNDEFINED &&
    target_supports_sse2) {
    sse2 = TRUE;
  }

  // Use SSE3 on systems that prefer it.
  if (target_prefers_sse3 &&
      sse2 != FALSE &&
      sse3 != FALSE) {
    sse2 = TRUE;
    sse3 = TRUE;
  }

#if 0 //temporarily disable it until we have assembler and linker support for
      //sse4a instructions
 // Use SSE4a on systems that supports it.
  if (target_supports_sse4a &&
      sse2 != FALSE &&  
      sse4a != FALSE){//not explicitly turned off
    sse2 = TRUE;
    sse4a = TRUE;
  }
#endif
 
  // No error.  Don't count warnings as errors.
  return TRUE;
}
#endif

#ifdef KEY /* Bug 11265 */
static void
accumulate_isystem(char *optargs)
{
  if (!isystem_dirs) {
    isystem_dirs = init_string_list();
  }
# define INCLUDE_EQ "-include="
  char *temp = malloc(strlen(optargs) + sizeof INCLUDE_EQ);
  add_string(isystem_dirs, strcat(strcpy(temp, INCLUDE_EQ), optargs));
}
#endif /* KEY Bug 11265 */

// Find the target name from the CPU name.
//   sysroot_abi ==  0  -->  --sysroot=name
//   sysroot_abi == -1  -->  --sysroot-n32=name
//   sysroot_abi ==  1  -->  --sysroot-64=name
static void
set_sysroot(char *name, int sysroot_abi)
{
#ifndef TARG_MIPS
  if (parsing_default_options && sysroot_path) {
    drop_option = TRUE;
  } else {
    if (sysroot_path) {
      warning("sysroot %s conflicts with %s; using latter (%s)",
	      sysroot_path, name, name);
      free(sysroot_path);
    }
    sysroot_path = string_copy(name);
  }
#else
  if (parsing_default_options &&
      (sysroot_path_n32 || sysroot_abi > 0) &&
      (sysroot_path_64  || sysroot_abi < 0)) {
    drop_option = TRUE;
    return;
  }
  // TODO: Add warnings
  if (sysroot_abi <= 0) {
    if (! parsing_default_options || sysroot_path_n32 == NULL) {
      if (sysroot_path_n32) free(sysroot_path_n32);
      sysroot_path_n32 = string_copy(name);
    }
  }
  if (sysroot_abi >= 0) {
    if (! parsing_default_options || sysroot_path_64 == NULL) {
      if (sysroot_path_64) free(sysroot_path_64);
      sysroot_path_64 = string_copy(name);
    }
  }
#endif
}

static void add_hugepage_desc
(
    HUGEPAGE_ALLOC alloc,
    HUGEPAGE_SIZE  size,
    int            limit
)
{
    HUGEPAGE_DESC desc;

    if (((alloc == ALLOC_BDT) || (alloc == ALLOC_BSS))
        && (limit != HUGEPAGE_LIMIT_DEFAULT))
        warning("Can't set huge page limit for %s in the command line.  Use HUGETLB_ELF_LIMIT instead",
                hugepage_alloc_name[alloc]);

    /* check whether to override existing descriptors */

    for (desc = hugepage_desc; desc != NULL; desc = desc->next) {
        if (desc->alloc == alloc) {
            if ((desc->size != size) || (desc->limit != limit)) {
                warning("conflict values for huge page %s; using latter values",
                        hugepage_alloc_name[alloc]);
            }

            desc->size = size;
            desc->limit = limit;
            return;
        }
    }
    
    desc = malloc(sizeof(HUGEPAGE_DESC_TAG));
    
    desc->alloc = alloc;
    desc->size = size;
    desc->limit = limit;

    desc->next = hugepage_desc;
    hugepage_desc = desc; 
}

static void
Process_Hugepage_Default()
{
    add_hugepage_desc(HUGEPAGE_ALLOC_DEFAULT, HUGEPAGE_SIZE_DEFAULT, HUGEPAGE_LIMIT_DEFAULT);
    add_option_seen(O_HP);
}

static boolean hugepage_warn = FALSE;

static void
Process_Hugepage_Group(char * hugepage_args)
{
    char * p = hugepage_args;
    boolean has_err = FALSE;
    int process_state = 0;
    HUGEPAGE_ALLOC hugepage_alloc;
    HUGEPAGE_SIZE  hugepage_size;
    int hugepage_limit;

    /* set default values */
    hugepage_alloc = HUGEPAGE_ALLOC_DEFAULT;
    hugepage_size = HUGEPAGE_SIZE_DEFAULT;
    hugepage_limit = HUGEPAGE_LIMIT_DEFAULT;
    
    while (*p) {
        if (process_state == 1) {
            if (strncmp(p, "2m", 2) == 0) {
                hugepage_size = SIZE_2M;
                p += 2;
                process_state = 2;
            }
            else if (strncmp(p, "1g", 2) == 0) {
                hugepage_size = SIZE_1G;
                p += 2;
                process_state = 2;
            }
            else
                has_err = TRUE;
        }
        else if (process_state == 2) {
            if (strncmp(p, "limit=", 6) == 0) {
                boolean is_neg = FALSE;
                p = &p[6];

                if ((*p) && ((*p) == '-')) {
                    p++;
                    is_neg = TRUE;
                }

                if (!(*p) || !isdigit(*p))
                    has_err = TRUE;
                else {
                    sscanf(p, "%d", &hugepage_limit);

                    if (is_neg && (hugepage_limit > 0))
                        hugepage_limit = -1;
                    
                    while ((*p) && ((*p) >= '0') && ((*p) <= '9'))
                        p++;
                    
                    process_state = 3;
                }
            }
            else
                has_err = TRUE;
        }
        else if (strncmp(p, "heap=", 5) == 0) {
            p = &p[5];
            hugepage_alloc = ALLOC_HEAP;
            process_state = 1;
            if (!(*p))
                has_err = TRUE;
            else
                continue;
        }
        else if (strncmp(p, "bdt=", 4) == 0) {
            p = &p[4];
            hugepage_alloc = ALLOC_BDT;
            process_state = 1;
            if (!(*p))
                has_err = TRUE;
            else
                continue;
        }
        else
            has_err = TRUE;

        if (*p) {
            if ((*p) == ',') {
                if (!has_err)
                    p++;

                if ((process_state != 2) || !(*p))
                    has_err = TRUE;
            }
            else if ((*p) == ':') {
                if (!has_err) {
                    p++;
                    add_hugepage_desc(hugepage_alloc, hugepage_size, hugepage_limit);
                }
                
                hugepage_alloc = HUGEPAGE_ALLOC_DEFAULT;
                hugepage_size = HUGEPAGE_SIZE_DEFAULT;
                hugepage_limit = HUGEPAGE_LIMIT_DEFAULT;
                process_state = 0;
            }
        }
        else if (!has_err) 
            add_hugepage_desc(hugepage_alloc, hugepage_size, hugepage_limit);

        if (has_err) {
            if (!hugepage_warn) {
                hugepage_warn = TRUE;
                warning("Illegal argument: %s in -HP", p);
            }
            break;
        }
    }

    if (!has_err) 
        add_option_seen(O_HP);
}

#include "opt_action.i"
