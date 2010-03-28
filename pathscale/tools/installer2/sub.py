#!/usr/bin/python
def parse_sub_keywd ( sub_lines, keywd ):
   for line in sub_lines:
        koff = line.find (keywd)
        if koff != -1:
           kvoff = line.find ('"', koff+len(keywd))
           kvoff = kvoff + 1
           kvoff2 = line.find ('"', kvoff)
           return line[kvoff:kvoff2].split(',')

sub_f = open ('/home/les/subs/pscsubscription-5965.xml','r')
sub_lines = sub_f.readlines()
sub_f.close()
sub_lines = ['< products value ="Compiler" >']
print 'products value is:' , parse_sub_keywd (sub_lines, "products value")

