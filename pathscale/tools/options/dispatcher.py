# Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
#
# Unpublished -- rights reserved under the copyright laws of the
# United States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION
# OR DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
# TRADE SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION IS
# PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF
# PATHSCALE, INC.
#
# U.S. Government Restricted Rights:
# The Software is a "commercial item," as that term is defined at 48
# C.F.R. 2.101 (OCT 1995), consisting of "commercial computer
# software" and "commercial computer software documentation," as such
# terms are used in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48
# C.F.R. 12.212 and 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE
# 1995), all U.S. Government End Users acquire the Software with only
# those rights set forth in the accompanying license
# agreement.
#
# PathScale, Inc. 2071 Stierlin Court, Suite 200; Mountain View CA 94043.

'''This is a multithreaded command dispatcher.  '''

__author__ = "David Allison <dallison@pathscale.com>"


import time
import os
import sys
import threading

class Dispatcher:
    lock = threading.Condition()
    def __init__ (self, silent=False):
        self.silent = silent
        self.commandqueue = None
        self.queuesize = 0
        self.runningcommands = 0
        self.running = True

    class Command:
        def __init__ (self, cmd, object, text, env):
            self.cmd = cmd
            self.object = object
            self.text = text
            self.next = None
            self.env = env

    def getCommand(self):
        self.lock.acquire()
        while self.running and self.queuesize == 0:
            self.lock.wait()
        if not self.running:
            return None,0
        self.queuesize -= 1
        comm = self.commandqueue
        self.commandqueue = comm.next
        self.runningcommands += 1
        njobs = self.runningcommands
        self.lock.release()
        return comm, njobs

    def putCommand (self, command, object, text, env):
        self.lock.acquire()
        cmd = self.Command (command, object, text, env)
        cmd.next = self.commandqueue
        self.commandqueue = cmd
        self.queuesize += 1
        self.lock.notify()
        self.lock.release()

    def commandComplete (self):
        self.runningcommands -= 1

    def complete (self):
        return self.runningcommands == 0 and self.queuesize == 0

    def stop (self):
        self.lock.acquire()
        self.running = False
        self.lock.notify()
        self.lock.release()

class Runner (threading.Thread):
    def __init__ (self, disp):
        threading.Thread.__init__(self)
        self.running = False
        self.disp = disp

    def run (self):
        self.running = True
        while self.running:
            command, jobno = self.disp.getCommand()
            if not self.running or command == None:
                break
            print '[' + str(jobno) + '] ' + command.text
            args = command.cmd.split()
            pid = os.fork()
            if pid == 0:
                for var in command.env.items():
                    key, val = var
                    os.putenv (key, val)
                try:
                    os.execv (args[0], args)
                except OSError, err:
                    print >> sys.stderr, ('Failed to run program: %s' %
                                          err.strerror)
                    sys.exit(1)
            else:
                wpid, status = os.waitpid (pid, 0)
                command.object.commandComplete (status)
                self.disp.commandComplete()


dispatcher = None
runners = []

def startDispatcher (nthreads, silent=False):
    global dispatcher
    global runners
    dispatcher = Dispatcher (silent)
    for i in range(nthreads):
        runner = Runner (dispatcher)
        runner.setDaemon(True)
        runner.start()
        runners.append (runner)


