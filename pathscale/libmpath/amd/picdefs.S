## Copyright 2005 PathScale, Inc.  All Rights Reserved.

## This file smooths over differences between PIC and non-PIC assembly

#ifdef PIC
#define EXTERN(a) a##@PLT
#else
#define EXTERN(a) a
#endif
