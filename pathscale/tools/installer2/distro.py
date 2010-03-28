#!/usr/bin/python

import os, sys, string
from platform import machine

class Distro:

# class constants
     redhat_compat = ['centos', 'hpc', 'redhat', 'sl']
     redhat_supported = range(4,6)
     fedora_compat = redhat_compat 
     fedora_compat.append('fedora')
     fedora_supported = range(3,9)
     suse_compat = ['openSUSE', 'suse', 'sles']
     suse_supported = range(9,11)
     ubuntu_compat = ['Gubuntu', 'Ubuntu', 'Kubuntu', 'Xubuntu', 'Edubuntu']
     ubuntu_supported = range(6,8)
     all_distros = fedora_compat + suse_compat + ubuntu_compat
     DISTRO_TYP_FEDORA = 'fedora'
     DISTRO_TYP_SUSE   = 'suse'
     DISTRO_TYP_UBUNTU = 'ubuntu'
     DISTRO_TYP_UNSUPPORTED   = 'unsupported'

     def fetch_releaseinfo( self ) :
         #handle Ubuntu releases separately because can't guarantee rpm
         f = open('/etc/issue','r')
         for line in f :
             parts = line.split()
             if len(line) > 1:
                name = parts[0]
                ver =  parts[1]
                if name in Distro.ubuntu_compat :
                   distro_releases = list()
                   distro_releases.append(name + '-release:' + ver)
                   f.close()
                   return distro_releases
         query = 'rpm -qa --queryformat "%{NAME}:%{VERSION}\n" "*-release*"'
         query_pipe = os.popen (query)
         distro_releases = list()
         for line in query_pipe.readlines():
             distro_releases.append(line.rstrip('\n'))
         exit_status = query_pipe.close()
         return distro_releases 

     def determine_type( self, distro_name ):
         if distro_name   in self.fedora_compat :
            self.type = Distro.DISTRO_TYP_FEDORA
         elif distro_name in self.suse_compat :
            self.type = Distro.DISTRO_TYP_SUSE
         elif distro_name in self.ubuntu_compat :
            self.type = Distro.DISTRO_TYP_UBUNTU
         else : 
            self.type = Distro.DISTRO_TYP_UNSUPPORTED

     def determine_arch (self) :
         m = machine()
         if m in ['i386','i486','i686'] : 
              return 'ia32'
         elif m == 'x86_64':
              return 'x86_64' 
         return 'unknown'

     def __init__ ( self ):
         
         for distro_info in self.fetch_releaseinfo():
             distro_pieces = distro_info.split(':')
             distro_name, self.ver  = distro_pieces
             self.name = distro_name.split('-')[0]
             if self.name in self.all_distros: 
                break
             else :
                self.name = "Unknown"
         self.determine_type ( self.name )  

         if self.ver.isdigit():
            self.subver = 0
         else :
            self.subver = self.ver.lstrip(string.digits)
            self.ver = self.ver[:-len(self.subver)]

         self.ver = int(self.ver)
         self.arch = self.determine_arch( )

     # distro_info
     #
     # Returns a <type>, <name>, <version> tuple for the distribution.
     
     def info(self):
         """Return a tuple of distribution type, name, and version."""
     
         return self.type, self.name, self.ver, self.arch
     
     # supported
     #
     # return True if this distro is supported and False if it is not.
     
     def supported (self):
         """Can this software distribution name and version be installed on this distro?"""
     
 
         if self.name ==  'fedora' :
            return (self.ver in self.fedora_supported) 
     
         if self.name in self.redhat_compat :
            return (self.ver in self.redhat_supported)
     
         if self.name in self.suse_compat :
             return (self.ver in self.suse_supported)
     
         if self.type == Distro.DISTRO_TYP_UBUNTU :
            return (self.ver in self.ubuntu_supported)

         return False
     
if __name__=='__main__':
    distro = Distro( )
    print distro.info( )
    #sys_name, sys_ver, soft_name, soft_ver
    print distro.software_ok ("fedora")
