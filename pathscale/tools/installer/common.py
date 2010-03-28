from ui import display_centered, radio_box, selection_box, fill_bar, text_box, text_entry_field 
debug_log = None
import os, os.path, sys
tmpdir = ''

class RunOpts:
    tmpdir = ''
    def get_tmpdir ( self ) :
        return self.tmpdir
    
    def __init__ ( self ):
        '''get -t working-temp-dir from program args'''   
        opt = 'nope'
        lim = len(sys.argv) 
        if lim < 2: return 
        for i in range(1,lim-1) : 
            if sys.argv[i] == '-t':
               self.tmpdir =  sys.argv[i+1]
               break;
        return
    
#
### Convert pathname to absolute path name
#
def pname_abs( pn ):
    '''Convert pathname to absolute pathname'''
    if os.path.isabs(pn) :
       return pn
    sh_pipe = os.popen( 'echo -n ' + pn )
    res = sh_pipe.readline()
    sh_pipe.close()
    if os.path.isabs(res) :
       return res
    else :
       return os.path.abspath(res)
