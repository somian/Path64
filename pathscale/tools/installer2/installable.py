#!/usr/bin/python
import time, os, sys, tempfile, socket, shutil, pprint
from os.path import abspath, basename, dirname, exists
from glob import glob
from tar import untar 
import pkg

# Constants
#
# (well, as constant as Python gets)

compiler_ver = '3.2'
default_dir = '/opt/pathscale'
sub_file_prefix = 'pscsubscription'
cwd = os.getcwd()
import common
runopts = common.RunOpts ( )
tmpdir = runopts.get_tmpdir ( )
recordit = None
recordit = open (tmpdir  + '/recordit.txt','a')

from distro import Distro

distro = Distro( )

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

class Installable :

# find_installables
#
# Returns a tuple of arrays of ISO locations, tar file locations,
# non-root tar file locations, and rpm directory locations found at
# the array of directories passed as search_path.

    installable_top_dir = 'pathscale-eko-compilers-' + compiler_ver
    
    fn_distros = { Distro.DISTRO_TYP_FEDORA : ['Fedora', 'fedora3','rpm'],
                   Distro.DISTRO_TYP_SUSE   : ['SUSE9', 'suse9.1','rpm'],
###  FIXME: Fill in UBUNTU   
                   Distro.DISTRO_TYP_UBUNTU : ['UNSUPPORTED','UNSUPPORTED','deb'],
                   Distro.DISTRO_TYP_UNSUPPORTED : ['UNSUPPORTED','UNSUPPORTED','rpm'] }
    fn_distro = fn_distros[distro.type]
    pkg_suffix = fn_distro[2]

    # downloaded from web tar files start with ...
    web_prefix = 'PathScale-Compiler-Suite-' + compiler_ver + '-' + fn_distro[0]
    web_tar_fn = web_prefix + '.tar'
    web_nr_tar_fn = web_prefix + '-nonroot.tar'
    
    iso_fn_glob =  'PathScale-Compiler-Suite-' + compiler_ver + '-[.0-9]*.iso' 
    
#   non-root tarball install filename pattern
    tarballs_fn_glob = 'pathscale-compilers-' + fn_distro[1] + '-' + compiler_ver +  '-[.0-9]*.tar.bz2'
    
    sub_serve_fn_glob = 'pathscale-sub-server-' + compiler_ver + '-[.0-9]*_psc.i386.' + pkg_suffix
    
    pkgs_fn_glob = 'pathscale-*' + compiler_ver + '-[.0-9]*_' + fn_distro[1] + '_psc.*.' + pkg_suffix
    
    def __init__ (self, search_path, root_access):
        """Find any installable downloads I can."""
    
        self.isos = list()
        self.web_tars= list()
        self.web_nr_tars= list()
        self.tars = list()
        self.nr_tars = list()
        self.pkgdirs = list()
        self.tarballs = list()
        
        self.sub_server_pkgs = pkg.create_pkg_set( distro.type )
        
        more_dirs = list()
        for dir in search_path:
            more_dirs.append(os.path.join ( dir, self.installable_top_dir ))
        search_path = search_path + more_dirs
        for dir in search_path:
            self.isos = self.isos + glob(os.path.join (dir, Installable.iso_fn_glob  ) ) 
            if basename(dir) == Installable.installable_top_dir :
                new_nr_dir = dir
            else :
                new_nr_dir = os.path.join (dir, Installable.installable_top_dir)
            self.tarballs = self.tarballs + (
                 glob(os.path.join (new_nr_dir, Installable.fn_distro[0], 'tar', self.tarballs_fn_glob  ) ) )
            self.web_tars = self.web_tars + glob(os.path.join (dir, Installable.web_tar_fn  ) )
            self.web_nr_tars = self.web_nr_tars + glob(os.path.join (dir, Installable.web_nr_tar_fn  ) ) 
            self.pkgdirs = self.pkgdirs + glob(os.path.join (dir, Installable.fn_distro[0], Installable.pkg_suffix) ) 

        self.web_tars  = unique(self.web_tars)
        for t in self.web_tars :
            new_pkgdir = os.path.join (tmpdir, self.installable_top_dir)
            if not os.path.exists( new_pkgdir ) and root_access :
               untar (t,tmpdir)
               self.pkgdirs = self.pkgdirs + (
                 glob(os.path.join (new_pkgdir, Installable.fn_distro[0], Installable.pkg_suffix + 's') ) )
        self.pkgdirs = unique(self.pkgdirs)

        self.web_nr_tars = unique(self.web_nr_tars)
        for t in self.web_nr_tars :
            new_nr_dir = os.path.join (tmpdir, self.installable_top_dir)  
            if not os.path.exists( new_nr_dir ) :
               untar (t,tmpdir)
               self.tarballs = self.tarballs + (
                 glob(os.path.join (new_nr_dir, Installable.fn_distro[0], 'tar', self.tarballs_fn_glob  ) ) )
        self.tarballs = unique(self.tarballs)


        self.isos = unique(self.isos)
        for dir in search_path:
               for fn in glob(os.path.join (dir, Installable.fn_distro[0], 'subscription-server')  + '/' + Installable.sub_serve_fn_glob) :
                   if fn not in  self.sub_server_pkgs:
                      self.sub_server_pkgs.append (fn)
            #recordit.write('checking dir ' + dir + '\n')
            #recordit.write(os.path.join (dir, Installable.fn_distro[0], 'subscription-server')  + '/' + Installable.sub_serve_fn_glob  + '\n')
            #recordit.write(str(glob(os.path.join (dir, Installable.fn_distro[0], 'subscription-server')  + '/' + Installable.sub_serve_fn_glob))  + '\n')
        #for x in self.sub_server_pkgs :
        #   recordit.write('server pkg =' + x + '\n')


def main (stdscr):
    """Main loop for the installer."""
    


# Application setup.
if __name__=='__main__':
    sys_state = initial_sys_state()
    installable = sys_state.installable
    for i in  [ installable.isos, installable.web_tars, installable.web_nr_tars, 
                installable.tars, installable.nr_tars, installable.pkgdirs, 
                installable.tarballs, installable.sub_servers ] :
                print i
    inst_selection = inst_set()
