#!/usr/bin/python
#
# Copyright 2008 PathScale, Inc.  All Rights Reserved.
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

########################################################################################
########################################################################################
##  pkg.py contains the
##  Pkgs base class
##  A pkgs is a package set that is a list of pkg_entry(s)
##       A pkg_entry has the path, base package name, ...
##  that can be installed, removed, updated, plus (because it is a list)
##    appended to, ...
##  RPMs and DEBs are types of package sets that implement 
##     install, remove, etc.
##  
##  pkg.Create_pky_set( distro_type)
##     is the package set factory method
##  It creates the right type of package set: DEBs for Ubuntu, RPMs for
##  other distro types.
########################################################################################
########################################################################################

import os, sys, string
from os.path import abspath, dirname, exists
from platform import machine

__metaclass__ = type

from distro import Distro

# create_pkg_set
#
# This is the class factory for a package
 
def create_pkg_set( distro_type ):
    if distro_type == Distro.DISTRO_TYP_UBUNTU :
       return DEBs( )
    else :
       return RPMs( )

#
# Installed_Path
#
#  has the INSTALLPREFIX top level directory and version

class Installed_Path:
    def __init__ ( self, prefix, ver ) :
        self.prefix = prefix    
        self.ver = ver
    def __str__ (self ) :
        return self.prefix + '   ' + self.ver

# pkg_entry
#
# Note: basename is not necessarily unique! (e.g. pathscale-pathdb)
# Use pkg_path to determine uniqueness.
    
class pkg_entry:
    "Entry for a single installable RPM."
    basename = ''
    pkg_path = ''
    found = False
    selected = False
    sub_present = False

    def __init__ (self, bn, p, f, s, sp):
        self.basename = bn
        self.pkg_path = p
        self.found = f
        self.selected = s
        self.sub_present = sp

    def __str__ ( self) :
        return pkg_path

# Miscellaneous utilities

# credit: Tim Peters, Python Cookbook, 17.3, Removing Duplicates
# assume s is a list of strings
def unique(s) :
    '''Return a list of elements in arbitrary order, but without duplicates.'''

    # take care of empty list case
    if len(s) == 0 : return []

    u = {}
    for x in s:
        u[x] = 1
    return u.keys()

class Pkgs (list):

  is_64bit = machine( ) == "x86_64"

  def __init__ (self):
      self.suffix = "PKG"
      self.txt = "PKGS"

#path = %{NAME}-%{VER}-.*%{PLATFORM}\.%{PKG_TYPE}
#       bn has %{NAME}
#       VER STARTS WITH DIGIT
#       PKG_TYPE is "rpm" or "deb"
  def append ( self, path ):
      """append pkg_entry for path to its list"""
#     sets have unique elements
      dup = False
      for p in self :
          if path == p.pkg_path :
             dup = True
             break
      if dup:
         return self
      bn = ""
      pieces = os.path.basename(path).split('-')
      for p in pieces :
         if p[0].isdigit() :
            ver = p
            break
         else :
            bn += p + '-'
      bn = bn.rstrip('-')
      lastp = pieces[-1].split('.')
      pkg_type = lastp[-1] 
      plat = lastp[-2]
      dist = lastp[-3].split('_')[1]
      new_pkg = pkg_entry( bn, path, True, False, False )
      return super(Pkgs, self).append( new_pkg )

  def install (self, prefix ):
    """install package set"""
    print 'not yet implemented'

  def remove (self):
    """remove package set"""
    print 'not yet implemented'

  def update (self, prefix ):
    """update package set"""
    print 'not yet implemented'

  def installed (path):
    print 'not yet implemented'

  def find_prev_inst ():
    """Find previous installations of the compiler packages."""

    print 'not yet implemented'
  find_prev_inst = staticmethod (find_prev_inst)

  # member function
  def basename (pkg_path):
    print 'not yet implemented'

  def file_installed (path):
    """Return True if the file is owned by an pkg, and False if it is not."""
    print 'not yet implemented'
    return False
  file_installed = staticmethod(file_installed)
   
  def matching (prefix, ver):
    """Return a list of package basenames """
    print 'not yet implemented'
    return list ()
  matching = staticmethod(matching) 

class RPMs (Pkgs):
  def __init__ (self):
      super (RPMs,self).__init__()
      self.suffix = "rpm"
      self.txt = "RPMS"

# install
#
# Install the rpms in the first argument with an optional prefix in
# the second argument.

  def install (self, prefix):
    """Install the rpm set at prefix."""

# installing on 32-bit machine requires ignoring architecture differences
    if is_64bit :
       force = ''
    else :
       force = ' --force '

    if prefix and prefix != '':
        rpm_cmd = 'rpm -i --quiet ' + force + ' --nodeps --prefix ' + prefix + ' ' + ' '.join(self)
    else:
        rpm_cmd = 'rpm -i --quiet ' + force + ' --nodeps ' + ' '.join(self)

    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status


# remove
#
# Remove the rpms in the argument.

  def remove (self):
    """Remove the rpm set."""

    rpm_cmd = 'rpm -e --quiet --nodeps '  + ' '.join(self)

    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status

# upgrade_rpm_set
#
# Upgrade the rpm in the first argument with an optional prefix in
# the second argument.

  def update (self, prefix):
    """Upgrade the rpm set at prefix."""

    if prefix and prefix != '':
        rpm_cmd = 'rpm -U --quiet --force --nodeps --prefix ' + prefix + ' ' + ' '.join(self)
    else:
        rpm_cmd = 'rpm -U --quiet --force --nodeps ' + ' '.join(self)

    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status


  def file_installed (path):
    """Return True if the file is owned by an pkg, and False if it is not."""
    print "RPM version"
    pkg_query = 'rpm -qf ' + path
    pkg_pipe = os.popen (pkg_query)
    pkg_result = pkg_pipe.readlines()
    pkg_pipe.close()

    if len (pkg_result) == 0 or pkg_result[0].find ('not owned') != -1 or pkg_result[0].startswith ('error:'):
        return False

    return True
  file_installed = staticmethod(file_installed)

  def find_prev_inst ():
    """Find previous installations of the compiler rpms."""

    ret_list = list()
    rpm_query = 'rpm -q --queryformat \'%{INSTALLPREFIX} %{VERSION}\\n\' pathscale-compilers-libs'
    rpm_pipe = os.popen (rpm_query)
    rpm_result = rpm_pipe.readlines()
    rpm_pipe.close()

    if len(rpm_result) == 0 or rpm_result[0].find('not installed') != -1:
        return ret_list

    for line in rpm_result:
        # Cut off the trailing newline
        cutline = line[:-1]
        if cutline == '(none)':
##### BUG: what to do here #######
            ret_list.append (Installed_Path(default_dir, compiler_ver ))
        else:
            prefix, ver = cutline.split()
            ret_list.append (Installed_Path(prefix, ver))


    return ret_list

  find_prev_inst = staticmethod (find_prev_inst)

# pkg_basename
#
# Return the pkg_basename of the rpm filename given.

  def pkg_basename (pkg_path):
    query = 'rpm -qp --queryformat \'%{NAME}\' ' + pkg_path

    query_pipe = os.popen (query)
    query_lines = query_pipe.readlines()
    query_pipe.close()

    if len(query_lines) > 0:
        return query_lines[0]
    else:
        return None

  pkg_basename = staticmethod (pkg_basename)

  def matching (prefix, ver):
    """Return a list of rpm basenames """
    ret_list = list()
    rpm_query = 'rpm -qa --queryformat \'%{NAME}-%{VERSION}-%{RELEASE}:%{INSTALLPREFIX}:%{VERSION}:\\n\''
    rpm_pipe = os.popen (rpm_query)
    rpm_result = rpm_pipe.readlines()
    rpm_pipe.close()

    for line in rpm_result:
        line_pieces = line.split(':')
        if ( line_pieces[0].startswith ('pathscale') and
            line_pieces[1] == prefix and
            line_pieces[2] == ver ):
                ret_list.append(line_pieces[0])

    return ret_list
  matching = staticmethod(matching) 

class DEBs(Pkgs):
  def __init__(self):
      super(DEBs,self).__init__ ()
      self.suffix = "deb"
      self.txt = "DEBS"
# install
#
# Install the debian packages in the first argument with an optional prefix in
# the second argument.


  def install (self, prefix):
    """Install the deb set at prefix."""

# installing on 32-bit machine requires ignoring architecture differences
    if is_64bit :
       force = ''
    else :
       force = ' --force-architecture '

    if prefix and prefix != '':
        deb_cmd = 'dpkg -i --quiet ' + force + '--nodeps --prefix ' + prefix + ' ' + ' '.join(self)
    else:
        deb_cmd = 'dpkg -i --quiet ' + force + '--nodeps ' + ' '.join(self)

    deb_pipe = os.popen (deb_cmd)
    deb_result = deb_pipe.readlines()
    exit_status = deb_pipe.close()

    return exit_status



# remove
#
# Remove the debian packages in the argument.

  def remove (self):
    """Remove the deb package set."""

# FIXME: what is equiv of quiet ?
    deb_cmd = 'dpkg  --quiet --force-depends --purge'  + ' '.join(self)

    deb_pipe = os.popen (deb_cmd)
    deb_result = deb_pipe.readlines()
    exit_status = deb_pipe.close()

    return exit_status

# update
#
# Update the deb in the first argument with an optional prefix in
# the second argument.

  def update (self, prefix):
    """Upgrade the deb set at prefix."""
# FIXME: update command is not implemented 
# I'm not sure if --update-avail or --merge-avail  is correct

# installing on 32-bit machine requires ignoring architecture differences
    if is_64bit :
       force = ''
    else :
       force = ' --force-architecture '

    if prefix and prefix != '':
        deb_cmd = 'dpkg --quiet ' + force + ' --force-depends --prefix ' + prefix + ' --update-avail ' + ' '.join(self)
    else:
        deb_cmd = 'dpkg --quiet ' + force + ' --force-depends ' + '--update-avail ' +  ' '.join(self)

    deb_pipe = os.popen (deb_cmd)
    deb_result = deb_pipe.readlines()
    exit_status = deb_pipe.close()

    return exit_status

# file_installed
#
# determines if file is already installed

  def file_installed (path):
    """Return True if the file is owned by an installed pkg, and False if it is not."""
    print "DEB version"
    pkg_query = 'dpkg-query --search' + path
    pkg_pipe = os.popen (pkg_query)
    pkg_result = pkg_pipe.readlines()
    pkg_pipe.close()

    if len (pkg_result) == 0 or pkg_result[0].find ('not found') != -1 or pkg_result[0].startswith ('error:'):
        return False

    return True
  file_installed = staticmethod(file_installed)


  def find_prev_inst ():
    """Find previous installations of the compiler rpms."""

    ret_list = list()
    rpm_query = 'dpkg-query -f \'${INSTALLPREFIX} ${Version}\\n\' --show  \'pathscale-compilers-libs\''
    rpm_pipe = os.popen (rpm_query)
    rpm_result = rpm_pipe.readlines()
    rpm_pipe.close()

    if len(rpm_result) == 0 or rpm_result[0].find('not installed') != -1:
        return ret_list

    for line in rpm_result:
        # Cut off the trailing newline
        cutline = line[:-1]
        if cutline == '(none)':
##### BUG: what to do here #######
            ret_list.append (Installed_Path(default_dir, compiler_ver ))
        else:
            prefix, ver = cutline.split()
            ret_list.append (Installed_Path(prefix, ver))


    return ret_list

  find_prev_inst = staticmethod (find_prev_inst)

