#!/usr/bin/python
#
# Copyright 2006. QLogic Corporation.
#
# Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
# Unpublished -- rights reserved under the copyright laws of the United
# States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION OR
# DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
# SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION IS
# PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF PATHSCALE,
# INC.
#
# U.S. Government Restricted Rights:
# The Software is a "commercial item," as that term is defined at 48
# C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
# and "commercial computer software documentation," as such terms are used
# in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
# 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
# End Users acquire the Software with only those rights set forth in the
# accompanying license agreement. PathScale, Inc. 2071 Stierlin Ct.,
# Suite 200, Mountain View, CA 94043.

# This is meant to be a sort of an MVC (model, view, controller)
# approach to a user interface. However, it's curses, and you can't
# really separate the view and the controller, so all that stuff is in
# the "_view" functions.

from sys import exit
try:
   import curses
except (ImportError),e:
   print e
   print  'It is contained in the python-curses rpm' 
   print  'If you are on a SuSE-type distribution, root can issue\n\t "yast2 --install python-curses"\n to install.'
   exit(1)

import time, os, sys, tempfile, socket, shutil, pprint
from os.path import dirname, abspath

from common import *

# install_tarball
#
# Installs the tarball at the prefix. That's all for now.

def untar (tarball, prefix=None):
    """Install the tarball """
    
    if prefix == None :
       prefix = os.path.dirname(tarball)

    if not os.path.exists (tarball) or not os.path.exists (prefix):
        return

    start_dir = os.getcwd()

    os.chdir (prefix)
    untar_cmd = '/bin/tar -xf ' + tarball
    untar_pipe = os.popen (untar_cmd)
    untar_result = untar_pipe.readlines()
    exit_status = untar_pipe.close()

    os.chdir (start_dir)

if __name__=='__main__':
   untar( '/home/les/Z0928-install/PathScale-Compiler-Suite-3.0.99-Fedora-nonroot.tar') 
