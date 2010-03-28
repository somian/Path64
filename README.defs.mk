# Licensed under the creative commons license
# Copyright (c) PathScale Inc. 2009


# The configure script will eventually generate 
# a complete and hopefully working defs.mk for multiple platforms
# This document will describe the way all options are handled 
# in the Makefiles

# Naming convention logic is as follows
# NEW == New way that depends on defs.mk
# L == Local - This was a small prefix that was 
# inheirited from prior build system
# The rest should be familiar

# Example
NEWLCFLAGS= -m32
NEWLCXXFLAGS= -m32
NEWLLDFLAGS= -m32

# LDFLAGS, CXXLDFLAGS and any other inconsistent variant are 
# all being replaced with $(NEWLLDFLAGS)
# The reason is simple.  Leave the existing mess in place and
# slowly refactor it out and not depend on it.
# Also I see no reasonable reason why a c object and c++ would link 
# differently. This seems uncommon and until there's an explicit
# need for it, it's being removed

# External libelf.. *joy*
LIBELF_A = /path/to/libelf.a

# When you want to build the native only lib version for libhugetlbfs
BUILD_LIB_TYPE = NATIVEONLY

# Default C compiler
NEWLCC = gcc

# Build compiler GNU or pathcc
BUILD_COMPILER := GNU
