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


int
NAMEf_dfnf (struct TYPEdfnf_struct *b)
{
	static ftnint i = 0;
	return(NAMEf_dfnf1(b, &i));
}

int
NAMEf_dfnf1 (struct TYPEdfnf_struct *b, ftnint *mask)
{
   char            nbuf[10];
   TYPEolist           a;
   int n, rlflag;

#if SIZEOF_LUNO_IS_64
   (void) sprintf (nbuf, "fort.%" SCNd64 "", b->unit);
#else
   (void) sprintf (nbuf, "fort.%d", b->unit);
#endif
   a.oerr = 0;
   a.ounit = b->unit;
   a.ofnm = nbuf;
   a.ofnmlen = (int) strlen (nbuf);
   a.osta = NULL;
   a.oacc = "d";
   a.oorg = "r";
   a.ofm = "u";
   a.occ = "n";
   a.orl = b->recl << 1;
   a.oblnk = NULL;
   a.oassocv = b->assocv;
   a.odisp = NULL;
   a.omaxrec = b->maxrec;
   a.orectype = NULL;
   a.odfnm = NULL;
/* 
 * Fix BN 11785 
 * Set the readonly switch to zero.
 * ---ravi---2/20/92
 */
   a.oreadonly = 0;
   /* Make sure that the OLD_RL flag is turned on so that the length
   will be interpreted as number of bytes */
   rlflag = f77vms_flag_[OLD_RL];
   f77vms_flag_[OLD_RL] = 1;
   n = f_open1 (&a, mask);
   f77vms_flag_[OLD_RL] = (unsigned short) rlflag;
   return(n);
}
