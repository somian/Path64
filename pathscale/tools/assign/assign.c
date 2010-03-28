/*
 * Copyright 2004, 2005, 2006 PathScale Inc.  All Rights Reserved.
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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fortran.h>
#include <cray/assign.h>

/*
 * Check if a string is a simple integer - i.e. that all elements in
 * the string are digits.
 */

void
check_int(char *s, char *o, int *error)
{
    int i;

    for(i = 0; i < strlen(s); i++) {
        if(!isdigit(s[i])) {
            fprintf(stderr, "Illegal %s value: %s is not an integer.\n", o, s);
            *error = 1;
            return;
        }
    }
}


void
check_on_off(char *s, char *o, int *error)
{
    if(strcmp(s, "on") && strcmp(s, "off")) {
        fprintf(stderr, "%s is not a valid setting for %s.  "
                "Valid settings are on or off.\n", s, o);
        *error = 1;
    }
}


void
parse_F_spec(char *class, int *error)
{
    char *type = NULL, *subtype = NULL;
    char *num1, *num2 = NULL, *num3 = NULL;
    char *c = strdup(class);

    num1 = index(c, ':');
    if(num1) {
        *num1 = '\0';
        num1 = num1 + 1;
        num2 = index(num1, ':');
        if(num2) {
            *num2 = '\0';
            num2 = num2 + 1;
            num3 = index(num2, ':');
            if(num3) {
                *num3 = '\0';
                num3 = num3 + 1;
            }
        }
    }

    type = index(c, '.');
    if(type) {
        *type = '\0';
        type = type + 1;
        subtype = index(type, '.');
        if(subtype) {
            *subtype = '\0';
            subtype = subtype + 1;
        }
    }

    if(strcmp(c, "blankx") && strcmp(c, "blx") &&
       strcmp(c, "bufa") && strcmp(c, "cache") &&
       strcmp(c, "cachea") && strcmp(c, "cos") &&
       strcmp(c, "blocked") && strcmp(c, "event") &&
       strcmp(c, "f77") && strcmp(c, "fd") &&
       strcmp(c, "global") && strcmp(c, "ibm") &&
       strcmp(c, "mr") && strcmp(c, "null") &&
       strcmp(c, "site") && strcmp(c, "stdin") &&
       strcmp(c, "stdout") && strcmp(c, "stderr") &&
       strcmp(c, "syscall") && strcmp(c, "system") &&
       strcmp(c, "text") && strcmp(c, "user") &&
       strcmp(c, "vms")) {
        fprintf(stderr, "Unknown FFIO class %s.\n", c);
        *error = 1;
    }

    if(num1) check_int(num1, "FFIO class numeric", error);
    if(num2) check_int(num2, "FFIO class numeric", error);
    if(num3) check_int(num3, "FFIO class numeric", error);
}


void
parse_F(char *s, int *error)
{
    char *current = s;

    do {
        char *comma, *nc;
        comma = index(current, ',');
        if(comma) {
            *comma = '\0';
            nc = comma + 1;
        } else {
            nc = &(current[strlen(current)]);
        }
        parse_F_spec(current, error);
        current = nc;
    } while (*current);
}


int
main (int argc, char **argv)
{
    int error = 0;
    int found_a = 0;
    int found_D = 0;
    int found_I = 0;
    int found_O = 0;
    int found_V = 0;
    int found_R = 0;
    int i, l = 0;
    char *str;
    FILE *ef;
    assign_environment aep;

    while(1) {
        int c;

        c = getopt(argc, argv, "a:b:f:m:s:tu:y:B:C:D:F:IN:ORS:T:U:VW:Y:Z:");
        if(c == -1) break;
        switch(c) {
        case 'a':
            found_a = 1;
            break;
        case 'b':
            check_int(optarg, "-b", &error);
            break;
        case 'f':
            if(strcmp(optarg, "77") && strcmp(optarg, "90")) {
                printf("%s is not a valid setting for -f.  Valid settings are "
                       "77 or 90.\n", optarg);
                error = 1;
            }
            break;
        case 'm':
            check_on_off(optarg, "-m", &error);
            break;
        case 's':
            if(strcmp(optarg, "bin") && strcmp(optarg, "cos") &&
               strcmp(optarg, "blocked") && strcmp(optarg, "sbin") &&
               strcmp(optarg, "text") && strcmp(optarg, "u") &&
               strcmp(optarg, "unblocked")) {
                printf("%s is not a valid setting for -s.\nValid settings are "
                       "bin, cos, blocked, sbin, text, u or unblocked.\n",
                       optarg);
                error = 1;
            }
            break;
        case 't':
            break;
        case 'u':
            check_int(optarg, "-u", &error);
            break;
        case 'y':
            check_on_off(optarg, "-y", &error);
            break;
        case 'B':
            check_on_off(optarg, "-B", &error);
            break;
        case 'C':
            if(strcmp(optarg, "ascii") && strcmp(optarg, "(no-op)") &&
               strcmp(optarg, "ebcdic")) {
                fprintf(stderr, "%s is not a valid setting for -C.  "
                        "Valid settings are ascii, (no-op) or ebcdic.\n",
                        optarg);
                error = 1;
            }
            break;
        case 'D':
            if(strcmp(optarg, "stdin") && strcmp(optarg, "stdout") &&
               strcmp(optarg, "stderr")) {
                fprintf(stderr, "%s is not a valid setting for -D.  "
                        "Valid settings are stdin, stdout or stderr.\n",
                        optarg);
                error = 1;
            }
            found_D = 1;
            break;
        case 'F':
            parse_F(optarg, &error);
            break;
        case 'I':
            found_I = 1;
            break;
        case 'N':
            if(strcmp(optarg, "cray") && strcmp(optarg, "ibm") &&
               strcmp(optarg, "vms") && strcmp(optarg, "ieee") &&
               strcmp(optarg, "ieee_32") && strcmp(optarg, "mips") &&
               strcmp(optarg, "ieee_64") && strcmp(optarg, "ieee_le") &&
               strcmp(optarg, "ultrix") && strcmp(optarg, "t3e") &&
               strcmp(optarg, "t3d") && strcmp(optarg, "user") &&
               strcmp(optarg, "site") && strcmp(optarg, "ia32") &&
               strcmp(optarg, "ia64") && strcmp(optarg, "be") &&
               strcmp(optarg, "bigendian")) {
                fprintf(stderr, "%s is not a valid setting for -N.  "
                        "Valid settings are cray, ibm, vms, ieee, ieee_32, "
                        "mips, ieee_64, ieee_le, ultrix, t3e, t3d, user, "
                        "site, ia32, ia64, be or bigendian.", optarg);
            }
            break;
        case 'O':
            found_O = 1;
            break;
        case 'R':
            found_R = 1;
            break;
        case 'S':
            check_on_off(optarg, "-S", &error);
            break;
        case 'T':
            check_on_off(optarg, "-T", &error);
            break;
        case 'U':
            check_on_off(optarg, "-U", &error);
            break;
        case 'V':
            found_V = 1;
            break;
        case 'W':
            check_on_off(optarg, "-W", &error);
            break;
        case 'Y':
            check_on_off(optarg, "-Y", &error);
            break;
        case 'Z':
            check_on_off(optarg, "-Z", &error);
            break;
        default:
            error = 1;
            break;
        }
    }

    if(found_a && found_D) {
        fprintf(stderr, "-a and -D options are mutually exclusive.");
        error = 1;
    }

    if(found_I && found_O) {
        fprintf(stderr, "-I and -O options are mutually exclusive.");
        error = 1;
    }

    if(optind != argc - 1 && !found_V && !found_R) {
        fprintf(stderr, "usage: %s options assign_object\n", argv[0]);
        error = 1;
    }

    /*
     * If we do "assign -R something" then add a -I to the mix.
     */

    if(optind == argc - 1 && found_R) {
        found_I = 1;
    }

    if(optind != argc) {
        if(!strncmp(argv[optind], "g:", 2)) {
            if(strcmp(argv[optind], "g:all") && strcmp(argv[optind], "g:su") &&
               strcmp(argv[optind], "g:sf") && strcmp(argv[optind], "g:du") &&
               strcmp(argv[optind], "g:df") && strcmp(argv[optind], "g:aq") &&
               strcmp(argv[optind], "g:ff")) {
                fprintf(stderr, "Illegal g: assign object %s.  Valid g: "
                        "options are g:all, g:su, g:sf, g:du, g:df, g:aq or "
                        "g:ff.\n", argv[optind]);
            error = 1;
            }
        } else if(!strncmp(argv[optind], "u:", 2)) {
            check_int(&(argv[optind][2]), "u:", &error);
        }
    }

    if(!getenv("FILENV")) {
        fprintf(stderr, "FILENV environment variable not set.\n");
        error = 1;
    }

    if(error) {
        return 1;
    }

    _ae_setupenv(&aep);
    _Ae_env_stack.env = &aep;
    _Ae_env_stack.size++;

    if(found_I || found_V) {
        ef = _gae_open('r', 'u', &error);
        if(ef == NULL && error) {
            fprintf(stderr, "Couldn't read assign file: %s.\n",\
                    strerror(errno));
            return 1;
        }

        if(ef) {
            _ae_internalize(ef, &aep);
            _gae_close(ef);
        }
    }

    if(found_V) {
        _ae_externalize(AE_ASGCMD, stdout, &aep);
        return 0;
    }

    for(i = 1; i < argc; i++)
        l += strlen(argv[i]) + 1;

    str = (char *)malloc(l + strlen("assign "));
    strcpy(str, "assign ");

    for(i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if(i != argc - 1) {
            strcat(str, " ");
        }
    }

    l = strlen(str);

    error = ffassign(str);

    ef = _gae_open('w', 'u', &error);
    if(ef == NULL) {
        fprintf(stderr, "Couldn't write assign file: %s.\n", strerror(errno));
        return 1;
    }

    _ae_externalize(AE_ASGCMD, ef, &aep);

    _gae_close(ef);

    return 0;
}
