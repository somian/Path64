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



#ifdef COMPILER_LICENSING
#include <stdio.h>
#include <stdlib.h>
#include <lmsgi.h>

#include <invent.h>
#include <sys/sysmp.h>

LM_CODE(code, ENCRYPTION_CODE_1, ENCRYPTION_CODE_2, VENDOR_KEY1,
                     VENDOR_KEY2, VENDOR_KEY3, VENDOR_KEY4, VENDOR_KEY5);


#define TRUE 1
#define FALSE 0
#define LINGER_PERIOD  10
#define ALL (-1)
#define ALL_8BIT (ALL & 0xFF)

/* ====================================================================
 *
 * get_license
 *
 * Get a license for automatic parallelization
 *
 * ====================================================================
 */

void
get_license( char * feature_name, int soft, int tiers )

{

 int err = FALSE;
 char * no_licensing;
 char * hard_licensing;
 char * tier_licensing;
 int numCPU;
 int archCPU;
 char *p;
 inventory_t *inv;
 int systemNumCPU = 0;
 int systemArchCPU;

 no_licensing = getenv("COMPILER_LICENSING");
 if (no_licensing != NULL) {
     return;
 }

 hard_licensing = getenv("HARD_LICENSING");
 if (hard_licensing != NULL) {
     soft = FALSE;
 }

 tier_licensing = getenv("TIER_LICENSING");
 if (tier_licensing != NULL) {
     tiers = FALSE;
 }

 if (license_init(&code,"sgifd",B_TRUE) < 0) {
    err = TRUE;
 }

 /* Set Up Soft Licensing */
 license_set_attr(LMSGI_NO_SUCH_FEATURE, NULL);

 /* Set the linger period to 10 seconds */
 license_set_attr(LM_A_LINGER, (LM_A_VAL_TYPE) LINGER_PERIOD);


 /*
    Attempt to check out a license
    leave the first argument as is.  you only need to fill in
    the feature name (program name) and version number.
 */

 if (license_chk_out(&code,     /* leave as is */
     feature_name,              /* replace with your feature name */
     "7.000"                    /* replace with your version number */
 )) {
      err = TRUE;
      fprintf(stderr,"%s\n", license_errstr());
      fprintf(stderr,"\n");
 }

 if ((tiers) && (err == FALSE)) {
     CONFIG *conf = lc_auth_data(get_job(), feature_name);
     if (conf == NULL) {
         err = TRUE;
         fprintf(stderr,"%s\n", license_errstr());
         fprintf(stderr,"\n");
     } else {
         if ((conf->users == 0) && (conf->lc_vendor_def != NULL) &&
             (strcmp(conf->lc_vendor_def, "") != 0)) {

           /* Nodelocked license with non empty vendor string */
           /* sscanf(conf->lc_vendor_def, "%d:%d", &numCPU, &archCPU); */

           p = conf->lc_vendor_def;
           while ( ((*p) != '\0')  && (((*p) < '0') || ((*p) > '9')) )
              p++;
           numCPU = atoi(p);

           while ((inv = getinvent()) != NULL) {
               if ((inv->inv_class == INV_PROCESSOR) &&
                   (inv->inv_type == INV_CPUBOARD)) {
                   if ((inv->inv_unit & ALL_8BIT) == ALL_8BIT) {
                       systemNumCPU = sysmp(MP_NPROCS);
                       systemArchCPU = inv->inv_state;
                   } else {
                       systemNumCPU = inv->inv_unit;
                       systemArchCPU = inv->inv_state;
                   }
               }
           }

           if ((numCPU >= systemNumCPU)) {
               err = FALSE;
           } else if ( numCPU == 0 ) {  /* A zero in the vendor string implies         */
               err = FALSE;             /* the largest system configuration tier       */
           } else {                     /* In other words, it will work on any system. */
               err = TRUE;
               fprintf(stderr, " This product's license is for %d CPU's; Your system has %d CPU's\n", numCPU, systemNumCPU );
               fprintf(stderr,"\n");
           }
        }
     }
 }

 if (err) {
   fprintf(stderr," This product (%s) requires a license password. \n", feature_name);
   fprintf(stderr," For license installation and trouble shooting \n");
   fprintf(stderr," information visit the web page: \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         http://www.sgi.com/Support/Licensing/install_docs.html \n");
   fprintf(stderr,"\n");
   fprintf(stderr," To obtain a Permanent license (proof of purchase\n");
   fprintf(stderr," required) or an Evaluation license please\n");
   fprintf(stderr," visit our license request web page: \n");
   fprintf(stderr,"\n");
#if 0
   fprintf(stderr,"    Internal SGI Users refer to:\n");
   fprintf(stderr,"         http://wwclass.csd.sgi.com/swl-internal/beta_lice.html\n");
   fprintf(stderr,"    Otherwise:\n");
#endif
   fprintf(stderr,"         http://www.sgi.com/Products/license.html \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         or send a blank email message to: \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         license@sgi.com \n");
   fprintf(stderr,"\n");
   fprintf(stderr," In North America, Silicon Graphics' customers may request \n");
   fprintf(stderr," Permanent licenses by sending a facsimile to: \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         (415) 390-0537 \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         or by calling our technical support hotline \n");
   fprintf(stderr,"\n");
   fprintf(stderr,"         1-800-800-4SGI \n");
   fprintf(stderr,"\n");
   fprintf(stderr," If you are Outside of North America or you are not a Silicon \n");
   fprintf(stderr," Graphics support customer then contact your local support provider. \n");
   fprintf(stderr,"\n");
#if 0
   fprintf(stderr," Note: Permanent Licenses require verification of entitlement \n");
   fprintf(stderr," (i.e., Proof-of-Purchase). \n");
   fprintf(stderr,"  \n");
#endif

  
   if (!soft)
      exit(-1);
 }
}

#endif
