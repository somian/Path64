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
   print  'If you are on a SuSE-type distribution, root can issue\n\t "yast --install python-curses"'
   print  '\t (some installations use yast2 for installing instead)'
   print  'to install.'
   exit(1)

import time, os, sys, tempfile, socket, shutil, pprint
from os.path import dirname, abspath

from common import *

from installable import Installable
# Constants
#
# (well, as constant as Python gets)

compiler_ver = '3.2.99'
default_dir = '/opt/pathscale'
sub_file_prefix = 'pscsubscription'

# Data structures
#
# An installation has three major components:
#
# 1. The state of the system before installation.
# 2. The set of software to be installed and configuration information
#    to be updated.
# 3. The results of the installation.
#
# These are represented by the following three classes:
#
# 1. initial_sys_state
#
#    Contains all the information about the system state, distribution,
#    software, and the location of the compiler payload.
#
# 2. inst_set
#
#    Contains the selections made by the user for the software to be
#    installed, the configuration changes to be made, and the
#    subscription information. The class may be instantiated with an
#    argument of the path to the saved installation information file.
#
# 3. inst_results
#
#    Contains the log of the installation results, and methods that
#    can reverse the installation. (?)

from distro import Distro

distro = Distro( )

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

class initial_sys_state:
    "Information about the system state at the start of the installation."
    root_access = False
    distro_type = distro.type
    distro_name = distro.name
    distro_version = distro.ver
    arch = 'x86_64'
    already_installed = False
    prev_tar_path_list = None
    prev_rpm_path_list = None
    
    # These are the possible ways the compiler can be delivered.
    iso_location = None
    # "web" means already downloaded to disk from the web
    web_tar_location = None
    web_nr_tar_location = None
    tar_location = None
    nr_tar_location = None
    rpm_location = None
    tarball_location = None
    sub_server_location = None

    # Locations of subscription files
    new_sub = None
    prev_sub_list = None

    # Location where the ISO is mounted. (This will be unmounted and removed.)
    mnt_pt = None
    
    def __init__(self):
        euid = os.geteuid()
        if euid == 0:
            self.root_access = True
        self.distro_type, self.distro_name, self.distro_version, junk = distro.info()
        # Since ubuntu does not use rpms, return before searching for installables
        # Long term we will update Installable() to support deb packages
        if self.distro_type == Distro.DISTRO_TYP_UBUNTU :
           return
        inst_search_path = list()
        inst_search_path.append(os.getcwd())
        if pname_abs(dirname(sys.argv[0])) not in inst_search_path :
           inst_search_path.append(pname_abs(dirname(sys.argv[0])))
        installables = Installable(inst_search_path, self.root_access  )
        self.iso_location = installables.isos
        self.web_tar_location = installables.web_tars
        self.web_nr_tar_location = installables.web_nr_tars
        self.tar_location = installables.tars
        self.nr_tar_location = installables.nr_tars
        self.rpm_location = installables.rpmdirs
        self.tarball_location = installables.tarballs
        self.sub_server_location = installables.sub_server_rpms
        self.arch = get_arch()
        
        # Find previous installations
        self.prev_tar_path_list = find_previous_tarinst ([default_dir])
        self.prev_rpm_path_list = find_previous_rpminst()
        if len(self.prev_tar_path_list) > 0 or len(self.prev_rpm_path_list) > 0:
            self.already_installed = True
        else:
            self.already_installed = False
        
        # Find subscription files
        sub_search_path = list()
        sub_search_path.append (os.getcwd())
        if os.environ.has_key ('HOME'):
            sub_search_path.append (os.environ['HOME'])
            sub_search_path.append (os.path.join (os.environ['HOME'], 'mail'))
            sub_search_path.append (os.path.join (os.environ['HOME'], 'Mail'))
            sub_search_path.append (os.path.join (os.environ['HOME'], '.pathscale'))
        self.new_sub = find_sub_file (sub_search_path)

        prev_rpm_prefix = list()
        for rpm_path in self.prev_rpm_path_list :
            prefix = rpm_path.prefix
            if prefix not in prev_rpm_prefix :
               prev_rpm_prefix.append (prefix)
        self.prev_sub_list = find_prev_subs (self.prev_tar_path_list, prev_rpm_prefix)

# rpm_entry
#
# Note: basename is not necessarily unique! (e.g. pathscale-pathdb)
# Use rpm_path to determine uniqueness.

class rpm_entry:
    "Entry for a single installable RPM."
    basename = ''
    rpm_path = ''
    found = False
    selected = False
    sub_present = False

    def __init__ (self, bn, p, f, s, sp):
        self.basename = bn
        self.rpm_path = p
        self.found = f
        self.selected = s
        self.sub_present = sp

class sub_info:
    "Information about the subscription."
    sub_path = None
    floating = False
    runtime_only = False
    c_allowed = False
    cpp_allowed = False
    f77_allowed = False
    f90_allowed = False
    products = list()
    
    def parse_sub_kwd ( self, sub_lines, kwd ):
       for line in sub_lines:
            koff = line.find (kwd)
            if koff != -1:
               kvoff = line.find ('"', koff+len(kwd))
               kvoff = kvoff + 1
               kvoff2 = line.find ('"', kvoff)
               return line[kvoff:kvoff2].split(',')
    
class inst_set:
    "The software to be installed and configurations to change."
    sub = None
    upgrade = False

    uninstall = False
    uninst_rpm_list = list()
    uninst_tar_list = list()
    
    inst_prefix = None
    rpm_list = list()
    tar_list = list()
    install_sub_server = False
    sub_server_path = None

    def __init__(self, startup_file = None):
        self.sub = sub_info()

    def add_rpm (self, basename, path, found, selected, allowed):
        new_rpm = rpm_entry (basename, path, found, selected, allowed)
        self.rpm_list.append (new_rpm)

    def parse_sub (self):
        try:
            sub_f = open (self.sub.sub_path)
        except:
            return
        sub_lines = sub_f.readlines()
        sub_f.close()
        for line in sub_lines:
            if line.find ('languages') != -1:
                line_pieces = line.split(' ')
                for piece in line_pieces:
                    if piece.startswith ('value'):
                        lang_string = piece[piece.find('"')+1:piece.rfind('"')]
        if lang_string:
            lang_pieces = lang_string.split(',')
            for piece in lang_pieces:
                if piece == 'C':
                    self.sub.c_allowed = True
                elif piece == 'CC':
                    self.sub.cpp_allowed = True
                elif piece == 'FORTRAN77':
                    self.sub.f77_allowed = True
                elif piece == 'FORTRAN90':
                    self.sub.f90_allowed = True

        self.sub.products = sub_info.parse_sub_kwd ( self.sub, sub_lines, "products value")   

class inst_results:
    "The results of the installation."
    inst_log = None
    rpm_record = None
    tar_record = None
    cfg_record = None

# Miscellaneous utilities

# get_arch
#
# Return the results of "uname -m." We can't use the Python "platform"
# module, as that is unavailable in 2.2.

def get_arch():
    """Return the machine architecture."""

    arch_pipe = os.popen ('uname -m')
    arch_result = arch_pipe.readlines()
    arch_pipe.close()

    if len(arch_result) > 0:
        return arch_result[0][:-1]
    else:
        return None


def arch_match (fn, arch):
    """Return true if the rpm in fn matches the architecture."""

    if (arch == 'i386' or
        arch == 'i586' or
        arch == 'i686'):
        if (fn.endswith ('.i386.rpm') or
            fn.endswith ('.i586.rpm') or
            fn.endswith ('.i686.rpm') or
            fn.endswith ('.noarch.rpm')):
            return True
    elif (arch == 'x86_64'):
        if (fn.endswith ('.i386.rpm') or
            fn.endswith ('.i586.rpm') or
            fn.endswith ('.i686.rpm') or
            fn.endswith ('.x86_64.rpm') or
            fn.endswith ('.noarch.rpm')):
            return True

    return False

# rpm_owned
#
# Return True if the file is owned by an rpm, and False if it is not.

def rpm_owned (path):
    """Return True if the file is owned by an rpm, and False if it is not."""

    rpm_query = 'rpm -qf ' + path
    rpm_pipe = os.popen (rpm_query)
    rpm_result = rpm_pipe.readlines()
    rpm_pipe.close()

    if len (rpm_result) == 0 or rpm_result[0].find ('not owned') != -1 or rpm_result[0].startswith ('error:'):
        return False

    return True

def rpms_matching (prefix, ver):
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

def rpm_installed (rpmname):
    """Return True if <rpmname> is installed, False if it is not."""

    rpm_query = 'rpm -q ' + rpmname
    rpm_pipe = os.popen (rpm_query)
    rpm_result = rpm_pipe.readlines()
    rpm_pipe.close()

    if len (rpm_result) == 0 or rpm_result[0].find ('not installed') != -1 or rpm_result[0].startswith ('error:'):
        return False

    return True

# find_previous_tarinst
#
# Find a previous installation or installations that were made from
# the tarball installer. We search for "pathcc," but any compiler
# binary would work, since they are all intalled by the tarball installer.

def find_previous_tarinst (search_path):
    """Find the previous installations that were made from the tarball installer."""

    ret_list = list()
    for dir in search_path:
        if os.path.isdir (dir):
            # TODO: If the path exists and is not owned by any rpm
            if os.path.exists (os.path.join (dir, 'bin/pathcc')) and not rpm_owned (os.path.join (dir, 'bin/pathcc')):
                ret_list.append (dir)

    return ret_list

# find_previous_rpminst
#
# returns a list of Installed_Path(s)
#
#

def find_previous_rpminst ():
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

# mount_iso
#
# Create a temporary mount point and mount the argument as a loopback
# device. Return the mount point on success, or None on failure.

def mount_iso (iso_fn, mount_pt_dir=''):
    """Mount the iso on a new temporary mount point."""

    if not os.path.exists (iso_fn):
        return None

    # Unfortunately, we are restricted to Python 2.2 and cannot use mkdtemp()
    if mount_pt_dir == '' :
        mount_pt_dir = tempfile.mktemp()

    if not os.path.exists (mount_pt_dir):
        os.mkdir (mount_pt_dir, 0700)

    mount_cmd = '/bin/mount -o loop ' + iso_fn + ' ' + mount_pt_dir
    debug_log.write( 'mount_cmd=' + mount_cmd + '\n')
    mount_pipe = os.popen (mount_cmd)
    mount_result = mount_pipe.readlines()
    exit_status = mount_pipe.close()

    if exit_status:
        os.rmdir (mount_pt_dir)
        return None
    else:
        return mount_pt_dir

# umount_iso
#
# Unmount the iso at mounted at the point specified in the argument,
# and remove the mount point.

def umount_iso (mnt_pt):
    """Unmount the iso at mnt_pt."""

    if not os.path.exists (mnt_pt):
        return

    umount_cmd = '/bin/umount ' + mnt_pt
    umount_pipe = os.popen (umount_cmd)
    umount_result = umount_pipe.readlines()
    exit_status = umount_pipe.close()

# install_tarball
#
# Installs the tarball at the prefix. That's all for now.

def install_tarball (tarball, prefix):
    """Install the tarball at prefix."""
    
    if not os.path.exists (tarball) or not os.path.exists (prefix):
        return

    start_dir = os.getcwd()

    new_dir = tempfile.mktemp()
    os.mkdir (new_dir, 0700)
    
    os.chdir (new_dir)
    untar_cmd = '/bin/tar xfj ' + tarball
    untar_pipe = os.popen (untar_cmd)
    untar_result = untar_pipe.readlines()
    exit_status = untar_pipe.close()

    dir_list = os.listdir (new_dir)
    for entry in dir_list:
        if entry.startswith ('pathscale'):
            os.rmdir (prefix)
            try:
                shutil.copytree (os.path.join (new_dir, entry), prefix, True)
            except:
                raise RuntimeError, 'Tar file installation failed. Cleanup needed at ' + prefix
            shutil.rmtree (os.path.join (new_dir, entry))
            break
    
    os.chdir (start_dir)
    if os.path.isdir (new_dir):
        os.rmdir (new_dir)

# upgrade_tarball
#
# "Upgrading" a tarball install is just moving the old tarball to a
# temporary location, unpacking the new one, and copying the
# subscription file from the old one (?).
#
# To do: Only remove the old tarball after an "undo" threshold has
# been passed.

def upgrade_tarball (tarball, prefix):
    """Upgrade the tarball at prefix."""
    
    if not os.path.exists (tarball) or not os.path.exists (prefix):
        return

# remove_tarball
#
# This removes the tarball at prefix. Really, that's just deleting the
# tree. We need to consider how we're going to undo this.

def remove_tarball (prefix):
    """Remove the tarball at prefix."""
    
    if not os.path.exists (prefix):
        return

    # Try to avoid blowing away /.
    if not os.path.exists (os.path.join (prefix, 'bin/pathcc')):
        return

    homedir = ''
    if os.environ.has_key ('HOME'):
        homedir = os.environ['HOME']

    if (prefix == '/' or
        prefix == '/usr' or
        prefix == '/usr/local' or
        prefix == '/var' or
        prefix == '/tmp' or
        prefix == '/etc' or
        prefix == '/dev' or
        prefix == '/home' or
        prefix == homedir):
        raise SystemError, 'You REALLY don\'t want to delete that directory!'

    nuke_cmd = 'rm -rf ' + prefix
    nuke_pipe = os.popen (nuke_cmd)
    nuke_result = nuke_pipe.readlines()
    nuke_pipe.close()

# install_rpm_set
#
# Install the rpms in the first argument with an optional prefix in
# the second argument.

def install_rpm_set (rpm_set, prefix = None):
    """Install the rpm set at prefix."""
    
    if prefix and prefix != '':
        rpm_cmd = 'rpm -i --quiet --force --nodeps --prefix ' + prefix + ' ' + ' '.join(rpm_set)
    else:
        rpm_cmd = 'rpm -i --quiet --force --nodeps ' + ' '.join(rpm_set)
    
    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status
    
# upgrade_rpm_set
#
# Upgrade the rpm in the first argument with an optional prefix in
# the second argument.

def upgrade_rpm_set (rpm_set, prefix = None):
    """Upgrade the rpm set at prefix."""

    if prefix and prefix != '':
        rpm_cmd = 'rpm -U --quiet --force --nodeps --prefix ' + prefix + ' ' + ' '.join(rpm_set)
    else:
        rpm_cmd = 'rpm -U --quiet --force --nodeps ' + ' '.join(rpm_set)
    
    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status
    
# remove_rpm_set
#
# Remove the rpms in the argument.

def remove_rpm_set (rpm_set):
    """Remove the rpm set."""

    rpm_cmd = 'rpm -e --quiet --nodeps '  + ' '.join(rpm_set)
    
    rpm_pipe = os.popen (rpm_cmd)
    rpm_result = rpm_pipe.readlines()
    exit_status = rpm_pipe.close()

    return exit_status

# find_sub_file
#
# Return the path to the first subscription file found in the search
# path.

def find_sub_file (search_path):
    """Return the path to the first subscription file found in the search path."""

    for dir in search_path:
	if os.path.exists (dir):
	    dir_list = os.listdir (dir)
	    for entry in dir_list:
		if entry.startswith (sub_file_prefix):
		    return os.path.join (dir, entry)

    return None

# find_prev_subs
#
# Create and return a list of subscription files found in previous
# installations. It will search all directories in <prefix>/lib for
# files beginning with the subscription prefix.

def find_prev_subs (tar_path, rpm_path):
    """Return a list of paths to previously installed subscription files."""

    sub_list = list()
    prev_path_list = list()
    if tar_path and len(tar_path) > 0:
        prev_path_list.extend (tar_path[:])
    if rpm_path and len(rpm_path) > 0:
        prev_path_list.extend (rpm_path[:])

    for d in prev_path_list:
        libdir = os.path.join (d, 'lib')
        if os.path.exists (libdir):
            l = os.listdir (libdir)
            for d2 in l:
                if os.path.isdir (os.path.join (libdir, d2)):
                    l2 = os.listdir (os.path.join (libdir, d2))
                    for entry in l2:
                        if entry.startswith (sub_file_prefix):
                            sub_list.append (os.path.join (libdir, d2, entry))

    return sub_list

# backup_sub_files
#
# Back up the subscription files found in the previous installations
# to a temporary directory and return that directory.

def backup_sub_files (sys_state):
    """Back up the subscription files found in the previous installations to a temporary directory and return that directory."""
    new_dir = tempfile.mktemp()
    try:
        os.mkdir (new_dir, 0700)
    except:
        raise RuntimeError, 'Could not create temporary directory ' + new_dir

    if sys_state.prev_sub_list and len (sys_state.prev_sub_list) > 0:
        for sub in sys_state.prev_sub_list:
            shutil.copy (sub, new_dir)

    return new_dir

# restore_sub_files
#
# Restore the subscription files from the backup directory and remove
# that directory. Don't pass this function a directory you would like
# to ever see again.

def restore_sub_files (inst_sel, backup_dir):
    """Restore the subscription files from the backup directory and remove that directory."""
    if os.path.isdir (backup_dir):
        l = os.listdir (backup_dir)
        for entry in l:
            if entry.startswith (sub_file_prefix):
                shutil.copy (os.path.join (backup_dir, entry), os.path.join (inst_sel.inst_prefix, 'lib', compiler_ver))

    shutil.rmtree (backup_dir)

# rpm_basename
#
# Return the basename of the rpm filename given.

def rpm_basename(rpm_path):
    query = 'rpm -qp --queryformat \'%{NAME}\' ' + rpm_path

    query_pipe = os.popen (query)
    query_lines = query_pipe.readlines()
    query_pipe.close()

    if len(query_lines) > 0:
        return query_lines[0]
    else:
        return None

# sub_allowed
#
# Given a string describing a software component and an inst_set instance,
# return True if the subscription allows that component, and False if it
# does not allow it, or if there is no subscription.

def sub_allowed (component, inst_sel):
    """Return whether or not a component is allowed by the subscription."""
    #comp_choice_list = ['C', 'C++', 'Fortran', 'Runtime Libraries', 'pathdb']

    if component == 'C' and inst_sel.sub.c_allowed == True:
        return True
    if component == 'C++' and inst_sel.sub.cpp_allowed == True:
        return True
    if component == 'Fortran' and (inst_sel.sub.f77_allowed == True or
                                   inst_sel.sub.f90_allowed == True):
        return True
    if component == 'Runtime Libraries':
        return True
    if component == 'pathdb' and (inst_sel.sub.c_allowed == True or
                                  inst_sel.sub.cpp_allowed == True or
                                  inst_sel.sub.f77_allowed == True or
                                  inst_sel.sub.f90_allowed == True):
        return True
    
    return False

# modify_ld_conf
#
# Modify /etc/ld.so.conf.d/pathscale-compilers.conf so that it contains the correct path to the
# libraries needed by the compiler.
#
# To do: Test inst_sel.uninstall, and if true, remove the lines starting
# with inst_sel.inst_prefix.

def modify_ld_conf (sys_state, inst_sel):
    """Fix up /etc/ld.so.conf.d/pathscale-compilers.conf for the user."""

    try:
    	conf_in = open ('/etc/ld.so.conf.d/pathscale-compilers.conf')
    except IOError: conf_lines = []
    else:
    	conf_lines = conf_in.readlines()
    	conf_in.close()

	if conf_lines[-1][-1] != '\n':
		conf_lines.append ('\n')
    if sys_state.arch == 'x86_64':
    	conf_lines.append(inst_sel.inst_prefix + '/lib/' + compiler_ver + '\n')
    conf_lines.append(inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32\n')

    try:
    	conf_out = open ('/etc/ld.so.conf.d/pathscale-compilers.conf', 'w')
    except:
    	return False

    conf_out.writelines (conf_lines)
    conf_out.close()

    return True


####################################################################
#                           Upgrade Check                          #
####################################################################

def check_upgrade_view (scr, sys_state, inst_sel):
    """View of the upgrade check page."""
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Welcome to the PathScale EKOPath Compiler Suite Installation')

    delta_y = text_box (scr, 5, 5, 4, max_x - 10, 'The PathScale EKOPath Compiler Suite is already installed on this system. Would you like to upgrade this installation, install version ' + compiler_ver + ' of the PathScale EKOPath Compiler Suite alongside the current installation, or uninstall the current installation?')

    display_centered (scr, max_y - 3, 'Press the up or down arrow to choose an option.')
    display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')

    upgrade_choice_list = ['Upgrade', 'Install Alongside', 'Uninstall']
    upgrade_choice = radio_box (scr, 6 + delta_y, 10, upgrade_choice_list)

    if upgrade_choice == 0:
        inst_sel.upgrade = True
    elif upgrade_choice == 2:
        inst_sel.uninstall = True

def check_upgrade (scr, sys_state, inst_sel):
    """Check to see if there is an installation to be upgraded."""
    
    check_upgrade_view (scr, sys_state, inst_sel)

def fresh_install_view (scr, sys_state, inst_sel):
    """View of the fresh install page."""
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Welcome to the PathScale EKOPath Compiler Suite Installation')

    delta_y = text_box (scr, 5, 5, 4, max_x - 10, 'The PathScale EKOPath Compiler Suite does not appear to be installed in the default location on this system. Would you like to perform a fresh installation of the compiler, or upgrade a compiler that is already installed at an alternate location?')

    display_centered (scr, max_y - 3, 'Press the up or down arrow to choose an option.')
    display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')

    install_choice_list = ['New Installation', 'Upgrade']
    install_choice = radio_box (scr, 6 + delta_y, 10, install_choice_list)

    if install_choice == 1:
        dy2 = text_box (scr, 9 + delta_y, 5, 2, max_x - 10, 'Please enter the path to the previous compiler installation.')

        old_tar =  text_entry_field (scr, 10 + delta_y + dy2, 10, max_x - 20)
        while len (find_previous_tarinst ([old_tar])) == 0:
            text_box (scr, 12 + delta_y + dy2, 5, 2, max_x - 10, 'Could not find a previous intallation in this location. Please enter the correct path, or just press enter to exit.')
            old_tar = text_entry_field (scr, 10 + delta_y + dy2, 10, max_x - 20)
            if old_tar == '':
                sys.exit(0)
        inst_sel.upgrade = True
        sys_state.prev_tar_path_list = [old_tar]
        sys_state.prev_sub_list.extend (find_prev_subs (sys_state.prev_tar_path_list, None))

def fresh_install (scr, sys_state, inst_sel):
    """Check to see if there is an installation to be upgraded."""
    
    fresh_install_view (scr, sys_state, inst_sel)

####################################################################
#                             Uninstall                            #
####################################################################

def uninstall_setup_view (scr, sys_state, inst_sel):
    """Interface for uninstalling the compiler."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Uninstall')

    dy = text_box (scr, 5, 5, 3, max_x - 10, 'Please select the compiler installations you would like to remove.')
    
    display_centered (scr, max_y - 3, 'Press the up or down arrow to choose an option.')
    display_centered (scr, max_y - 2, 'Press <Space> to toggle selection, <Enter> to accept selections.')

    uninst_choice_list = sys_state.prev_tar_path_list[:]
    if sys_state.root_access :
        for installed_path in sys_state.prev_rpm_path_list:
            uninst_choice_list.append (str(installed_path))
    uninst_choice_list.append ('Another location')

    rpm_index = len (sys_state.prev_tar_path_list)

    if sys_state.root_access :
        fillin_index = rpm_index + len (sys_state.prev_rpm_path_list)
    else :
        fillin_index = rpm_index 

    choices = selection_box (scr, dy + 6, 10, uninst_choice_list)

    tarballs_to_remove = list()
    rpms_to_remove = list()

    i = 0
    for c in choices:
        if c:
            if i < rpm_index:
                tarballs_to_remove.append (uninst_choice_list[i])
            else:
                if i < fillin_index:
                    seldir, selver = uninst_choice_list[i].split()
                    rpms_to_remove.append (rpms_matching (seldir, selver))
                else:
                    dy = text_box (scr, 14, 10, 3, max_x - 20, 'Please enter the path to the tarball installation you wish to uninstall, or blank to cancel.')
                    fillin_path = text_entry_field (scr, 15 + dy, 10, max_x - 20)
                    while not os.path.isdir (fillin_path) and fillin_path != '' and fillin_path.upper() != 'BLANK' and fillin_path != ' ':
                        display_centered (scr, 17 + dy, 'The directory could not be found.')
                        fillin_path = text_entry_field (scr, 15 + dy, 10, max_x - 20)

                    if fillin_path != '' and fillin_path.upper() != 'BLANK' and fillin_path != ' ':
                        tarballs_to_remove.append (fillin_path)
        i += 1

    display_centered (scr, max_y - 3, '                                                                ')
    display_centered (scr, max_y - 2, '                                                                ')
    display_centered (scr, max_y - 2, 'Removing compiler software...', curses.A_BOLD)
    scr.refresh()

    # TODO: Handle unsuccessful removal attempt.
    if not os.environ.has_key('DONT_INSTALL'):
        for tarball in tarballs_to_remove:
            remove_tarball (tarball)
        for rpm_set in rpms_to_remove:
            remove_rpm_set (rpm_set)
        
    display_centered (scr, max_y - 2, '                                                                ')
    display_centered (scr, max_y - 2, 'Press any key to continue.')
    scr.getch()
    return ( len(tarballs_to_remove) + len(rpms_to_remove) ) > 0

def uninstall_setup (scr, sys_state, inst_sel):
    """Help the user uninstall the compiler."""

    return uninstall_setup_view (scr, sys_state, inst_sel)

def uninstall_complete_view (scr, did_uninstall):
    """View of the uninstall completion screen."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()


    if did_uninstall :
        display_centered (scr, 2, 'Uninstall Complete')
        dy = text_box (scr, 5, 5, 3, max_x - 10, 'The PathScale Compiler Suite has been removed from the selected location(s).')
    else :
        display_centered (scr, 2, 'Uninstall Not Attempted')
        dy = text_box (scr, 5, 5, 3, max_x - 10, 'The PathScale Compiler Suite has not been removed because there were no selected location(s).')
    
    display_centered (scr, max_y - 2, 'Press any key to exit.')
    scr.getch()

def uninstall_complete (scr, did_uninstall):
    """Inform the user that the software removal was successful."""

    uninstall_complete_view (scr, did_uninstall)

####################################################################
#                         Installation Type                        #
####################################################################

# show_inst_choices_view
#
# Allow the user to choose which installation source he or she
# desires. Do not show the ISO list, as we have already given the
# option to mount an ISO.

def show_inst_choices_view (scr, sys_state):
    """Show the installable files I've found."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Installable Packages')
    
    text_box (scr, 4, 5, 4, max_x - 10, 'I have searched the local directory structure and found the following installable software packages. Packages listed with arrows are compatible with this system.')
    
    tmp_y = 8
    if len (sys_state.rpm_location) > 0:
        scr.addstr (tmp_y, 5, 'RPM Directories:')
        for entry in sys_state.rpm_location:
            tmp_y += 1
            scr.addstr (tmp_y, 6, '--> ' + entry, curses.A_BOLD)
        tmp_y += 1
    
    if len (sys_state.tar_location) > 0:
        scr.addstr (tmp_y, 5, 'Tar File Downloads:')
        for entry in sys_state.tar_location:
            tmp_y += 1
            scr.addstr (tmp_y, 6, '--> ' + os.path.basename(entry), curses.A_BOLD)
        tmp_y += 1
    
    if len (sys_state.nr_tar_location) > 0:
        scr.addstr (tmp_y, 5, 'Non-root Tar File Downloads:')
        for entry in sys_state.nr_tar_location:
            tmp_y += 1
            scr.addstr (tmp_y, 6, '--> ' + os.path.basename(entry), curses.A_BOLD)
        tmp_y += 1
    
    if len (sys_state.tarball_location) > 0:
        scr.addstr (tmp_y, 5, 'Tar Files:')
        for entry in sys_state.tarball_location:
            tmp_y += 1
            scr.addstr (tmp_y, 6, '--> ' + os.path.basename(entry), curses.A_BOLD)
        tmp_y += 1
    
    display_centered (scr, max_y - 2, 'Press any key to continue.')
    c = scr.getch()

# ask_mount_iso
#
# Ask the user to choose an ISO to mount, if desired. Add all
# installation sources found on the mounted ISO to the list of
# installables.

def ask_mount_iso (scr, sys_state):
    """Ask the user to choose an ISO to mount, if desired."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Select an ISO to Mount')

    text_box (scr, 5, 5, 4, max_x - 10, 'I have found the following ISO files on your system. Each of these contains a complete CD image. Which, if any, would you like to mount, so that you may select it as an installation source?')

    display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')

    iso_choice_list = list()
    for iso in sys_state.iso_location:
        iso_choice_list.append (os.path.basename (iso))
    iso_choice_list.append ('None of these ISO files.')

    iso_choice = radio_box (scr, 10, 10, iso_choice_list)
    if iso_choice >= len (sys_state.iso_location):
        return

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Where to Mount ISO?')

    dy2 = text_box (scr, 9, 5, 2, max_x - 10, 'Please enter the path for mounting the iso or Press Enter for mount to a temporary file:')
    mount_pt_dir = text_entry_field (scr, 9+dy2, 5, max_x - 10 )

    sys_state.mnt_pt = mount_iso (sys_state.iso_location[iso_choice], mount_pt_dir)

    if not sys_state.mnt_pt:
        text_box (scr, max_y - 6, 10, 3, max_x - 20, 'Could not mount ISO. Please check check the ISO file permissions and try again.\n If the ISO is on a remote filesystem, try copying it to a local disk. Press any key to exit.')
        display_centered (scr, max_y - 2, '                                  ')
        display_centered (scr, max_y - 2, 'Press any key to exit.')
        scr.getch()
        cleanup (sys_state, None)
        sys.exit (1)

    # Add all the installables found in the ISO to the installation
    # source choices.
    inst_search_path = list()
    inst_search_path.append (sys_state.mnt_pt)
    installables = Installable (inst_search_path, sys_state.root_access)
    sys_state.tar_location.extend (installables.tars[:])
    sys_state.nr_tar_location.extend (installables.nr_tars[:])
    sys_state.rpm_location.extend (installables.rpmdirs[:])
    sys_state.tarball_location.extend (installables.tarballs[:])
    sys_state.sub_server_location.extend (installables.sub_server_rpms[:])

# nothing_to_install
#
# Failure screen to indicate that I found nothing installable.

def nothing_to_install (scr):
    """Failure screen to indicate that I found nothing installable."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()
    
    display_centered (scr, 2, 'Nothing to Install')

    text_box (scr, 5, 5, 5, max_x - 10, 'I found no installable packages. Please verify that you have downloaded an installable package or packages and run this installer from the directory into which they were downloaded.')

    display_centered (scr, max_y - 2, 'Press any key to exit.')

    c = scr.getch()

# nothing_to_install_unless_root
#
# Failure screen to indicate that I found nothing installable.

def nothing_to_install_unless_root (scr):
    """Failure screen to indicate that I found nothing installable by a non-root user."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()
    
    display_centered (scr, 2, 'Nothing to Install')

    text_box (scr, 5, 5, 5, max_x - 10, 'The only installable packages I found required root access to install. Please either run this installer as root, or download packages that are installable as by a non-root user and run this installer from the directory into which they were downloaded.')

    display_centered (scr, max_y - 2, 'Press any key to exit.')

    c = scr.getch()

# get_inst_type_view
#
# Present the user with the possible installation types.

def get_inst_type_view(scr, sys_state, inst_selection):
    """Display installation types."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Installation Type')

    text_box (scr, 5, 5, 4, max_x - 10, 'The PathScale EKOPath Compiler Suite can be installed from RPMs or a non-root tar file. RPMs require root access, while the non-root tar file requires only write access to the target directory.')

    scr.addstr (9, 5, 'Which installation source would you prefer?')

    display_centered (scr, max_y - 3, 'Press the up or down arrow to choose an option, or <q> to quit.')
    display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')

    if sys_state.root_access:
        inst_choice_list = ['Tar File', 'RPMs']
    else:
        inst_choice_list = ['Tar File']
        text_box (scr, 12 + len (inst_choice_list), 5, 2, max_x - 10, 'You do not currently have root privileges. Root access is required to install RPMs.')
    inst_choice = radio_box (scr, 11, 10, inst_choice_list, 0, ['q','Q'])

    if inst_choice == -1:
        cleanup (sys_state, inst_selection)
        sys.exit(0)
    elif inst_choice == 0:
        # Fill the tar field of the installation set
        for entry in sys_state.tarball_location:
            inst_selection.tar_list.append(entry)
    elif inst_choice == 1:
        # Fill the rpms field of the installation set
        for entry in sys_state.rpm_location:
            for root in [entry, os.path.join(entry,'i386'), os.path.join(entry,'x86_86')] :
                if os.path.isdir(root) :
                    for rpm in os.listdir(root):                
                        if arch_match (rpm, sys_state.arch):
                            inst_selection.add_rpm (rpm_basename (os.path.join (root, rpm)), 
                                os.path.join (root, rpm), True, False, False)
    else:
        raise RuntimeError, 'Picked a nonexistent installation source.'

# get_inst_type
#
# Help the user decide if he or she will be installing RPMs or a
# tarball.

def get_inst_type(scr, sys_state, inst_selection):
    """Help user decide type of installation."""

    if len (sys_state.iso_location) > 0 and sys_state.root_access:
        ask_mount_iso (scr, sys_state)
    if ((len (sys_state.rpm_location) > 0 and sys_state.root_access) or
        (len (sys_state.tar_location) > 0 and sys_state.root_access) or
        len (sys_state.nr_tar_location) > 0 or
        len (sys_state.tarball_location) > 0):
        show_inst_choices_view (scr, sys_state)
    else:
        if (len (sys_state.rpm_location) > 0 or
            len (sys_state.tar_location) > 0 or
            len (sys_state.iso_location) > 0) and not sys_state.root_access:
            nothing_to_install_unless_root (scr)
            cleanup (sys_state, inst_selection)
            sys.exit (0)
        else:
            nothing_to_install (scr)
            cleanup (sys_state, inst_selection)
            sys.exit (0)
    get_inst_type_view (scr, sys_state, inst_selection)

####################################################################
#                            Subscription                          #
####################################################################

# set_sub_type_view

def set_sub_type_view(scr, inst_selection):
    """The view to the subscription type selection."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Subscription Type')

    dy = text_box (scr, 5, 5, 3, max_x - 10, 'The PathScale EKOPath Compiler Suite provides two different subscription options, floating and nodelocked. Most demonstration subscriptions are nodelocked.')
    
    scr.addstr (dy + 6, 5, 'Which subscription type have you purchased?')

    display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')

    sub_choice_list = ['Floating', 'Nodelocked', 'Runtime Only (No Subscription)']
    sub_choice = radio_box (scr, dy + 8, 10, sub_choice_list)

    if sub_choice == 0:
        inst_selection.sub.floating = True
        inst_selection.sub.runtime_only = False
    elif sub_choice == 1:
        inst_selection.sub.floating = False
        inst_selection.sub.runtime_only = False
    elif sub_choice == 2:
        inst_selection.sub.floating = False
        inst_selection.sub.runtime_only = True
    else:
        raise RuntimeError, 'Invalid subscription type.'

# set_sub_type
#
# Determine the type of subscription the customer want and collect
# necessary information.

def set_sub_type(scr, inst_selection):
    """Help user choose an appropriate subscription type."""

    set_sub_type_view (scr, inst_selection)

####################################################################
#                        System Configuration                      #
####################################################################

# determine_prereqs
#
# Determine which, if any, of the prerequisites are available on the
# system. Do not exit the installer or give errors - this provides
# information for the component selection step.

def determine_prereqs():
    """Determine what set of possible system prerequisites is present."""

####################################################################
#                        Components to Install                     #
####################################################################

# determine_components_view

def determine_components_view (scr, sys_state, inst_selection):
    """View for component selection screen."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Software Component Selection')

    dy = text_box (scr, 5, 5, 4, max_x - 10, 'Please select the software components you would like to install. If the software components you would like to install do not appear here, the prerequisites have not been met.');

    display_centered (scr, max_y - 3, 'Press the up or down arrow to choose an option.')
    display_centered (scr, max_y - 2, 'Press <Space> to toggle selection, <Enter> to accept selections.')
    
    comp_choice_list = ['C', 'C++', 'Fortran', 'Runtime Libraries', 'pathdb']

    y = dy + 6
    x = 10

    temp_y = y
    choices = [False] * len(comp_choice_list)
    selectable_y = list()
    
    for choice in comp_choice_list:
        if (sub_allowed (choice, inst_selection)):
            scr.addstr (temp_y, x, '[ ] ' + choice)
            selectable_y.append(temp_y)
        else:
            scr.addstr (temp_y, x, '[ ] ' + choice + ' (Need subscription)')
            # Remove the following line to disallow selection of components
            # for which the user has no subscription.
            selectable_y.append(temp_y)
        temp_y += 1

    min_y = selectable_y[0]
    max_y = selectable_y[-1]
    sel_index = 0
    scr.move (min_y, x+1)

    selection_made = False
    while not selection_made:
        cur_y, cur_x = scr.getyx()
        c = scr.getch()
        if c == curses.KEY_UP:
            if cur_y == min_y:
                curses.beep()
            else:
                sel_index -= 1
                scr.move (selectable_y[sel_index], cur_x)
        elif c == curses.KEY_DOWN:
            if cur_y == max_y:
                curses.beep()
            else:
                sel_index += 1
                scr.move (selectable_y[sel_index], cur_x)
        elif c == ord(' '):
            index = cur_y - min_y
            if choices[index] == True:
                scr.addstr (cur_y, cur_x, " ")
                scr.move (cur_y, cur_x)
                choices[index] = False
            else:
                scr.addstr (cur_y, cur_x, "X")
                scr.move (cur_y, cur_x)
                choices[index] = True
                # Select runtime libraries if anything else is selected.
                if index != 3:
                    if choices[3] == False:
                        scr.addstr (min_y + 3, cur_x, 'X')
                        scr.move (cur_y, cur_x)
                        choices[3] = True
        elif c == curses.KEY_ENTER or c == ord('\n'):
            selection_made = True

    # Consider moving this to its own function. It contains some
    # knowledge that is pretty difficult to extract.

    for rpm in inst_selection.rpm_list:
        if rpm.basename == 'pathscale-c' and choices[0]:
            rpm.selected = True
        elif rpm.basename == 'pathscale-c++' and choices[1]:
            rpm.selected = True
        elif rpm.basename == 'pathscale-f90' and choices[2]:
            rpm.selected = True
        elif rpm.basename == 'pathscale-pathdb' and choices[4]:
            rpm.selected = True
        elif ((rpm.basename == 'pathscale-base' or
               rpm.basename.startswith ('pathscale-compilers-') or
               rpm.basename == 'pathscale-pathopt2') and
              (choices[0] or choices[1] or choices[2] or choices[3] or choices[4])):
            rpm.selected = True
        elif (rpm.basename == 'pathscale-sub-client' and
              (choices[0] or choices[1] or choices[2] or choices[4])):
            rpm.selected = True

# determine_components
#
# Present the user a selection of installable components. The user
# selects zero or more of these components.

def determine_components(scr, sys_state, inst_selection):
    """Ask the user which components he or she wishes to install."""

    determine_components_view (scr, sys_state, inst_selection)

# get_inst_prefix_view
#
# TO DO: Scream if the user is installing (not upgrading) RPMs, and an
# installation is already present in the target directory.

def get_inst_prefix_view(scr, sys_state, inst_sel):
    """View of the installation prefix screen."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Installation Location')

    if not sys_state.root_access :
       prefix_choice = 0
       ln = 0
    else:
       ln = text_box (scr, 5, 5, 4, max_x - 10, 'By default, the PathScale EKOPath Compiler Suite installs into [b]' + default_dir + '.[/b] Would you like to specify an alternate directory?')
   
       display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')
   
       prefix_choice_list = ['Yes', 'No']
       prefix_choice = radio_box (scr, 7 + ln, 10, prefix_choice_list, 1)

    if prefix_choice == 0:
        text_box (scr, 11 + ln, 5, 2, max_x - 10, 'Please specify the installation directory:')
        inst_sel.inst_prefix = pname_abs(text_entry_field (scr, 13 + ln, 10, max_x - 20) )
    else:
        inst_sel.inst_prefix = default_dir
# TODO: if os.access (inst_sel.inst_prefix, os.W_OK):

def get_inst_prefix (scr, sys_state, inst_sel):
    """Ask the user if he or she wants to specify an alternate installation location."""

    get_inst_prefix_view (scr, sys_state, inst_sel)

# reselect_rpms
#
# Select rpms in sel.rpm_list for installation if previous versions of
# them are already present on the system.
#
# Selection rules:
#
# If any of C, C++, Fortran, or pathdb is installed, select every RPM
# except those of the four which are not installed. If none of those
# are installed, select runtime only.

def reselect_rpms (sel):
    c_installed = rpm_installed ('pathscale-c')
    cpp_installed = rpm_installed ('pathscale-c++')
    f_installed = rpm_installed ('pathscale-f90')
    dbg_installed = rpm_installed ('pathscale-pathdb')

    for rpm in sel.rpm_list:
        if rpm.basename == 'pathscale-c':
            if c_installed:
                rpm.selected = True
        elif rpm.basename == 'pathscale-c++':
            if cpp_installed:
                rpm.selected = True
        elif rpm.basename == 'pathscale-f90':
            if f_installed:
                rpm.selected = True
        elif rpm.basename == 'pathscale-pathdb':
            if dbg_installed:
                rpm.selected = True
        elif rpm.basename != 'pathscale-compilers-libs':
            if (c_installed or cpp_installed or f_installed or dbg_installed):
                rpm.selected = True
        else:
            rpm.selected = True

def setup_upgrade (sys_state, sel):
    """Silently make the right choices for an upgrade."""

    # If it's a tarball, they get a tarball again. If it's rpms, they
    # get the same set of rpms. No checking the subscription; I'm
    # going to trust them here.

    if len (sys_state.prev_rpm_path_list) > 0 and len (sys_state.rpm_location) > 0 and len (sel.rpm_list) > 0:
        # Figure out which ones are installed
        reselect_rpms (sel)
        sel.inst_prefix = sys_state.prev_rpm_path_list[0].prefix
    elif len (sys_state.prev_tar_path_list) > 0 and len (sys_state.tarball_location) > 0 and len (sel.tar_list) > 0:
### BUG doesn't tarball also need a list of Installed_Path(s)
        sel.inst_prefix = sys_state.prev_tar_path_list[0]
    else:
        # Return with an error that there's nothing to upgrade.
        return False

    return True

def upgrade_mismatch_view (scr, sys_state, sel):
    """View of the upgrade_mismatch function."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Upgrade mismatch')

    if len (sel.rpm_list) > 0:
        msg = 'You have attempted to upgrade a tarball installation to an RPM installation. Please restart the installer (as root) and select tarball install. If tarballs are not available, please download them from the PathScale support site.'
    else:
        msg = 'You have attempted to upgrade an RPM installation to a tarball installation. Please restart the installer (as root) and select RPM install. If RPMs are not available, please download them from the PathScale support site. '

    text_box (scr, 5, 5, 5, max_x - 10, msg)

    display_centered (scr, max_y - 2, 'Press any key to exit')
    scr.getch()

def upgrade_mismatch (scr, sys_state, sel):
    """Inform the user that he or she has attempted to upgrade with a
    non-matching set of installables."""

    upgrade_mismatch_view (scr, sys_state, sel)

# prefix_dir_nonempty
#
# Failure screen to indicate that I found the prefix directory non-empty

def prefix_dir_nonempty (scr, dir):
    """Failure screen to indicate that I found nothing installable."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()
    
    display_centered (scr, 2, 'installation prefix directory non-empty')

    text_box (scr, 5, 5, 5, max_x - 10, 'Directory ' + dir + ' non-empty.\nNothing has been installed yet.' )

    display_centered (scr, max_y - 2, 'Press any key to exit.')

    c = scr.getch()
    sys.exit(0)

####################################################################
#                        Perform Installation                      #
####################################################################

# IDEA:
#
# In order to present the user with the best indicator of how long the
# tarball installation is taken, we start by performing a "tar tvf" on
# the tarball, and track the number of lines in it. Then we untar with
# "tar xvf", counting the lines again, and use that to fill the
# progress bar.

# perform_inst_view

def perform_inst_view (scr, sys_state, inst_sel):
    """View of the installation progress."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Installation Progress')

    display_centered (scr, 5, 'Your software is now being installed.')

    display_centered (scr, 6, '(This can take a couple of minutes)')

    if os.environ.has_key('DONT_INSTALL'):
        if len (sys_state.prev_tar_path_list) > 0:
            display_centered (scr, 11, 'I AM REMOVING ' + sys_state.prev_tar_path_list[0])
            scr.refresh()
        for progress in [ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 ]:
            time.sleep (0.1)
            fill_bar (scr, 8, 5, max_x - 10, progress)
            scr.refresh()
    else:
        if inst_sel.upgrade:
            sub_backup_dir = backup_sub_files (sys_state)
            if len (sys_state.prev_tar_path_list) > 0:
                for tarpath in sys_state.prev_tar_path_list:
                    remove_tarball (tarpath)
        if len (inst_sel.tar_list) > 0:
            if not os.path.exists (inst_sel.inst_prefix):
                try:
                    os.makedirs (inst_sel.inst_prefix)
                except:
                    raise RuntimeError, 'Could not create target directory ' + inst_sel.inst_prefix
            scr.refresh()
# TODO: if told new install but inst_prefix is not empty, fails so catch that here and put up screen and exit.
#       len(os.listdir(inst_sel.inst_prefix) == 0 if ok to install_tarball
            if not inst_sel.upgrade and (
                len(os.listdir(inst_sel.inst_prefix)) > 0 ) :
                prefix_dir_nonempty(scr, inst_sel.inst_prefix)  
            install_tarball (inst_sel.tar_list[0], inst_sel.inst_prefix)
        else:
            # rpm install
            rpm_count = 0
            rpm_total = 0
            for rpm in inst_sel.rpm_list:
                if rpm.selected:
                    rpm_total += 1
            if rpm_total:
                for rpm in inst_sel.rpm_list:
                    if rpm.selected:
                        if inst_sel.upgrade:
                            upgrade_rpm_set ([rpm.rpm_path], inst_sel.inst_prefix)
                        else:
                            install_rpm_set ([rpm.rpm_path], inst_sel.inst_prefix)
                        rpm_count += 1
                        fill_bar (scr, 8, 5, max_x - 10, float (rpm_count) / float (rpm_total))
                        scr.refresh()
        if inst_sel.install_sub_server:
            if (inst_sel.sub_server_path and os.path.exists (inst_sel.sub_server_path)):
                display_centered (scr, 12, 'Installing subscription server...')
                scr.refresh()
                if inst_sel.upgrade:
                    upgrade_rpm_set ([inst_sel.sub_server_path], inst_sel.inst_prefix)
                else:
                    install_rpm_set ([inst_sel.sub_server_path], inst_sel.inst_prefix)
                display_centered (scr, 13, 'Done.', curses.A_BOLD)
                scr.refresh()
        if inst_sel.upgrade and sub_backup_dir:
            restore_sub_files (inst_sel, sub_backup_dir)

    if inst_sel.sub.sub_path and not os.environ.has_key('DONT_INSTALL'):
        new_sub_dir = os.path.join (inst_sel.inst_prefix, 'lib', compiler_ver)
        new_sub_fn = os.path.basename (inst_sel.sub.sub_path)
        new_sub_path = os.path.join (new_sub_dir, new_sub_fn)
        if inst_sel.sub.sub_path != new_sub_path :
            shutil.copy (inst_sel.sub.sub_path, new_sub_path)
            os.chmod (new_sub_path, 0644)
    display_centered (scr, max_y - 2, '                   ')
    display_centered (scr, max_y - 2, 'Press any key to continue.')
    c = scr.getch()

# perform_inst
#
# Actually perform the configured installation. Show the user progress
# and allow him or her to interrupt the process and back up.

def perform_inst (scr, sys_state, inst_sel):
    """Perform the configured installation."""

    perform_inst_view (scr, sys_state, inst_sel)

####################################################################
#                   Post-Installation Configuration                #
####################################################################



# lib_config
#
# tell user if there are missing libraries

def lib_config (sys_state, inst_sel):
    """Tell user if there are missing libraries"""
    if sys_state.distro_name in ['suse','sles']: 
       f.write( '\n*******************************************************\n')
       rpm_pipe = os.popen ('rpm -q glibc-devel-32bit')
       rpm_result = rpm_pipe.readlines()
       rpm_pipe.close()
       res = rpm_result[0][:-1]
   
       if  len(rpm_result) > 0 and res.endswith(" is not installed") : 
          f.write("Please install rpm glibc-devel-32bit")
       else:
          f.write( "Everything is fine.")
       f.write( '\n*******************************************************\n')
    return 

# selinux_config
#
# tell user if selinux changes are required

def selinux_config (sys_state, inst_sel):
    """Give directions on any selinux changes required."""

    if  not os.path.exists('/usr/sbin/selinuxenabled') : 
       return
    if os.system('/usr/sbin/selinuxenabled') == 0 :
       f.flush()
       #inst_sel.inst_prefix + '/lib/' + compiler_ver
       f.write( '\n' + '*'*65 + '\n' + 
9*':' + '\n' + 'ISSUE  ::\n' + 9*':' + 2*'\n' +
'''The PathScale compiler suite contains dynamic libraries that may be
flagged as security violations when SELinux is enabled. This can occur
when running the compiler itself and when running code generated by
the compiler. If SELinux is in conforming mode these security
violations will prevent the program from running and cause diagnostics
such as these:\n ''' + 
inst_sel.inst_prefix + '/lib/' + compiler_ver + '/gfec: error while loading shared libraries:\n' +
          inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32/libmv.so.1:' +
          'cannot restore segment prot after reloc: Permission denied\n' +
          'pathcc INTERNAL ERROR:' +  inst_sel.inst_prefix + '/lib/' + compiler_ver +
           '''/gfec returned non-zero status 127\n
Please report this problem to http://support.pathscale.com\n''' +
'\n' + 12*':' + '\n' + 'SOLUTION  ::\n' + 12*':' + 2*'\n'  +
'''Set the security policy on the affected .so files to allow text
   relocations. This can be achieved with the following shell command by
   the root user:
              chcon -t texrel_shlib_t `find ''' +
              inst_sel.inst_prefix + ''' -type f -name "*.so*" -print`
    
You can check that the setting has been applied correctly using this
  command:
              ls --lcontext `find ''' + 
           inst_sel.inst_prefix + '''-type f -name "*.so*" -print`
    
If you install the PathScale run-time libraries on other machines you
will need to reapply the chcon command on those machines.
 
*******************************************************
       ''' )
       f.flush()
       return

# post_config_view

def post_config_view (scr, sys_state, inst_sel):
    """Post-installation configuration screen."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'System Configuration')

    if len (inst_sel.tar_list) > 0:
        dy = text_box (scr, 5, 5, 5, max_x - 10, 'Your software has been successfully installed. The following system configuration changes are recommended.');

        if sys_state.root_access:
            if sys_state.arch == 'x86_64':
                scr.addstr (dy + 6, 10, 'Add the following lines to /etc/ld.so.conf.d/pathscale-compilers.conf:')
                scr.addstr (dy + 8, 20, inst_sel.inst_prefix + '/lib/' + compiler_ver)
                scr.addstr (dy + 9, 20, inst_sel.inst_prefix + '/lib/32/' + compiler_ver)
            else:
                scr.addstr (dy + 6, 10, 'Add the following line to /etc/ld.so.conf.d/pathscale-compilers.conf:')
                scr.addstr (dy + 8, 20, inst_sel.inst_prefix + '/lib/32/' + compiler_ver)
            text_box (scr, dy + 12, 5, 2, max_x - 10, 'Would you like me to make these changes for you?')
            display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')
            config_choice_list = ['Yes', 'No']
            config_choice = radio_box (scr, dy + 14, 10, config_choice_list)
            if config_choice == 0 and not os.environ.has_key('DONT_INSTALL'):
                modify_ld_conf (sys_state, inst_sel)
        else:
            f.write('\n' + '*'*60 + '\n')
            scr.addstr (dy + 6, 8, 'Set the LD_LIBRARY_PATH environment variable to (bash shell example):')
            f.write('Set the LD_LIBRARY_PATH environment variable to (bash shell example):\n\t')
            if sys_state.arch == 'x86_64':
                scr.addstr (dy + 8, 8, 'export LD_LIBRARY_PATH=' + inst_sel.inst_prefix + '/lib/' + compiler_ver + ':' + inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32:$LD_LIBRARY_PATH')
                f.write('export LD_LIBRARY_PATH=' + inst_sel.inst_prefix + '/lib/' + compiler_ver + ':' + inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32:$LD_LIBRARY_PATH')
            else:
                scr.addstr (dy + 8, 8, 'export LD_LIBRARY_PATH=' + inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32:$LD_LIBRARY_PATH')
                f.write('export LD_LIBRARY_PATH=' + inst_sel.inst_prefix + '/lib/' + compiler_ver + '/32:$LD_LIBRARY_PATH')
            f.write('\n' + '*'*60 + '\n')
            display_centered (scr, max_y - 2, 'Press any key to continue.')
            c = scr.getch()
    else:
        text_box (scr, 5, 5, 5, max_x - 10, 'Your software has been successfully installed. The RPMs have already made the appropriate system configuration changes.');
        display_centered (scr, max_y - 2, 'Press any key to continue.')
        c = scr.getch()

# post_config
#
# Configure the system to use the compiler, if possible. This includes
# modifying /etc/ld.so.conf.d/pathscale-compilers.conf, for example.

def post_config(scr, sys_state, inst_sel):
    """Configure the system to use the compiler."""
    
    #selinux_config (sys_state, inst_sel)   selinux changes are no longer needed
    lib_config (sys_state, inst_sel)
    post_config_view (scr, sys_state, inst_sel)

####################################################################
#                         Subscription Setup                       #
####################################################################

# sub_server_setup_view

def sub_server_setup_view (scr, sys_state, inst_sel):
    """View of the subscription server setup."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Floating Subscription Configuration')

    dy = text_box (scr, 5, 5, 7, max_x - 10, 'A floating subscription requires access to a machine running the subscription server. You may configure this system to be the subscription server, or specify the name of another system that is the subscription server. If that system has the hostname or alias "pscsubscriptionserver", it will be determined automatically.')

    if sys_state.root_access:
        dy2 = text_box (scr, dy + 6, 5, 2, max_x - 10, "Would you like to install the subscription server software on this system?")
        display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')
        
        sub_choice_list = ['Yes', 'No']
        sub_choice = radio_box (scr, dy + dy2 + 8, 10, sub_choice_list)
        if sub_choice == 0:
            inst_sel.install_sub_server = True
            if len(sys_state.sub_server_location) > 0:
                for file in sys_state.sub_server_location:
                    inst_sel.sub_server_path = file
        else:
            inst_sel.install_sub_server = False
    else:
        dy2 = text_box (scr, dy + 6, 5, 2, max_x - 10, "Root privileges are required to install the subscription server.")
        display_centered (scr, max_y - 2, 'Press any key to continue.')
        scr.getch()

# sub_server_setup

def sub_server_setup (scr, sys_state, inst_sel):
    """Set up the subscription server for those with a floating license."""

    sub_server_setup_view (scr, sys_state, inst_sel)

# sub_setup_view

def sub_setup_view (scr, sys_state, inst_selection):
    """Subscription configuration screen."""

    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Subscription Configuration')

    text_box (scr, 5, 5, 5, max_x - 10, 'The subscription will now be configured for this system.')

    if sys_state.new_sub:
        delta_y = text_box (scr, 7, 5, 3, max_x - 10, 'Found a subscription file found at ' + sys_state.new_sub + '. Would you like to use this subscription file?')
        display_centered (scr, max_y - 2, 'Press <Enter> to accept selection.')
        sub_choice_list = ['Yes', 'No']
        sub_choice = radio_box (scr, delta_y + 8, 10, sub_choice_list)
        if sub_choice == 0:
            inst_selection.sub.sub_path = sys_state.new_sub
        else:
            delta_y2 = text_box (scr, delta_y + 11, 5, 3, max_x - 10, 'Please specify the path to the subscription file:')
            text_box (scr, delta_y2 + 17, 5, 3, max_x - 10, 'Enter the complete location of the subscription file (e.g. /tmp/pscsubscription-Compiler-1234.xml).')
            sub_path = pname_abs(text_entry_field (scr, delta_y2 + 15, 5, max_x - 10))
            while not os.path.isfile (sub_path):
                text_box (scr, delta_y + 11, 5, 3, max_x - 10, 'File not found. Please specify a path to the subscription file:')
                sub_path = text_entry_field (scr, delta_y2 + 15, 5, max_x - 10)
            inst_selection.sub.sub_path = sub_path
    else:
        delta_y = text_box (scr, 7, 5, 3, max_x - 10, 'No subscription file found. Please specify the path to the subscription file:')
        text_box (scr, delta_y + 10, 5, 3, max_x - 10, 'Enter the complete location of the subscription file (e.g. /tmp/pscsubscription-Compiler-1234.xml).')
        sub_path = pname_abs(text_entry_field (scr, delta_y + 8, 5, max_x - 10))
        while not os.path.isfile (sub_path):
            text_box (scr, 7, 5, 3, max_x - 10, 'Could not find the subscription file. Please specify the path to the subscription file:')
            sub_path = pname_abs (text_entry_field (scr, delta_y + 8, 5, max_x - 10))
        inst_selection.sub.sub_path = sub_path

    inst_selection.parse_sub()

# subscription_setup
#
# Set up the subscription for the user, if necessary.

def subscription_setup (scr, sys_state, inst_selection):
    """Set up the subscription software."""

    sub_setup_view (scr, sys_state, inst_selection)


# verify_inst_view

def verify_inst_view (scr, verify_inst_info):
    """View of the installation verification."""
    
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Installation Verified')

    text_box (scr, 5, 5, 5, max_x - 10, 'The compiler installation on [b]' + socket.gethostname() + '[/b] was successful. You can now begin using the PathScale EKOPath Compiler Suite.')

    #display_centered (scr, max_y - 3, 'Exiting the installer will commit all changes.')
    display_centered (scr, max_y - 2, 'Press any key to exit.')

    c = scr.getch()

# verify_inst
#
# Verify that the installation has been successful, the subscription
# is correct, and the compiler (if installed) works.

def verify_inst (scr):
    """Verify the compiler and subscription installation."""

    verify_inst_info = None

    verify_inst_view (scr, verify_inst_info)

####################################################################
#                               Main                               #
####################################################################

def cleanup (sys_state, inst_sel):
    """Clean up after ourselves."""

    if sys_state.mnt_pt:
        umount_iso (sys_state.mnt_pt)
        os.rmdir (sys_state.mnt_pt)

def splash_screen(scr) :
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered (scr, 2, 'Pathscale Compiler Suite version ' + compiler_ver + 'Installation Script')

    text_box (scr, 5, 5, 5, max_x - 10, 'Determining initial setting and looking for installable software...')


def unsupported_distro( scr ):
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered( scr, 2, 'Your distribution is not supported.' )
    text_box (scr, 5, 5, 5, max_x - 10, 'Quitting. No changes have been made to your system.\nPlease press any key to exit.')

    c = scr.getch()


def ubuntu_distro( scr ):
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered( scr, 2, 'Your Ubuntu distribution can be installed using instructions in the release notes.' )
    text_box (scr, 5, 5, 5, max_x - 10, 'Quitting. No changes have been made to your system.\nPlease press any key to exit.')

    c = scr.getch()

def msg_initializing ( scr ):
    scr.clear()
    scr.box()
    max_y, max_x = scr.getmaxyx()

    display_centered( scr, 2, 'Determining initial system state' )
    text_box (scr, 5, 5, 5, max_x - 10, 'Note: This could take a minute or two if any tarballs need to be untarred.')


# Our main loop.
def main (stdscr):
    """Main loop for the installer."""
    
    msg_initializing ( stdscr )
    sys_state = initial_sys_state()
    if distro.type == Distro.DISTRO_TYP_UBUNTU :
        ubuntu_distro( stdscr )
        sys.exit (1) 
    if not distro.supported( ) :
        unsupported_distro( stdscr )
        sys.exit (1)        
    inst_selection = inst_set()

    try:
        if sys_state.already_installed:
            check_upgrade (stdscr, sys_state, inst_selection)
        else:
            fresh_install (stdscr, sys_state, inst_selection)
        if inst_selection.uninstall:
            did_uninstall = uninstall_setup (stdscr, sys_state, inst_selection)
            uninstall_complete (stdscr, did_uninstall)
            cleanup (sys_state, inst_selection)
            sys.exit (0)

        if not inst_selection.upgrade or (not sys_state.prev_sub_list or len (sys_state.prev_sub_list) == 0):
            set_sub_type (stdscr, inst_selection)
            if inst_selection.sub.floating:
                sub_server_setup (stdscr, sys_state, inst_selection)
            if not inst_selection.sub.runtime_only:
                subscription_setup (stdscr, sys_state, inst_selection)
                
        # If we're upgrading, we should know what to install and where
        # to put it.
        get_inst_type (stdscr, sys_state, inst_selection)
        if not inst_selection.upgrade:
            if len (inst_selection.rpm_list) > 0:
                determine_components (stdscr, sys_state, inst_selection)
            get_inst_prefix (stdscr, sys_state, inst_selection)
        else:
            if not setup_upgrade (sys_state, inst_selection):
                upgrade_mismatch (stdscr, sys_state, inst_selection)
                cleanup (sys_state, inst_selection)
                sys.exit (1)
        
        if (len (inst_selection.rpm_list) == 0 and
           len (inst_selection.tar_list) == 0 ) :
           nothing_to_install( stdscr )
           cleanup (sys_state, inst_selection)
           sys.exit(1)

        perform_inst (stdscr, sys_state, inst_selection)
        post_config (stdscr, sys_state, inst_selection)
        verify_inst (stdscr)
    except KeyboardInterrupt:
        cleanup (sys_state, inst_selection)
        sys.exit (1)
    
    cleanup (sys_state, inst_selection)

# Application setup.
if __name__=='__main__':
    runopts = RunOpts ( )
    tmpdir = runopts.get_tmpdir ( )
    if os.path.exists(tmpdir + '/psc-summary.txt') : 
       os.remove(tmpdir + '/psc-summary.txt')
    f = open (tmpdir + '/psc-summary.txt','a')
    if os.path.exists(tmpdir + '/debug_log.txt') : 
       os.remove(tmpdir + '/debug_log.txt')
    debug_log = open (tmpdir + '/debug_log.txt','a')
    curses.wrapper (main)
    f.close()
    debug_log.close ()
    print '\n'*24, 'Please issue command "more ' + tmpdir +'/psc-summary.txt" for any post-install tasks to do.', '\n'*10
