/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

#include "f2c.h"
#include "signal1.h"

void *
signal_ (integer * sigp, sig_pf proc)
{
  int sig;
  sig = (int) *sigp;

  return (void *) signal (sig, proc);
}
