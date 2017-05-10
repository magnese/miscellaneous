#!/usr/bin/env python

import os
import getpass

# parameters
outputname = "run"
outputnode = "01"
queue = "standard"
execname = "dune-navier-stokes-two-phase"
scriptname = "submit.sh"

# env parameters
user = getpass.getuser()
pwd = os.getcwd()

# generate script
f = open(scriptname,'w')
f.write("#PBS -N {}\n".format(outputname))
f.write("#PBS -m abe\n")
f.write("#PBS -q {}\n".format(queue))
f.write("#PBS -o /scratchcomp{0}/{1}/{2}/{2}.out\n".format(outputnode,user,outputname))
f.write("#PBS -e /scratchcomp{0}/{1}/{2}/{2}.err\n".format(outputnode,user,outputname))
f.write("mkdir -p /scratchcomp{0}/{1}\n".format(outputnode,user))
f.write("mkdir -p /scratchcomp{0}/{1}/{2}\n".format(outputnode,user,outputname))
f.write("cd /scratchcomp{0}/{1}/{2}\n".format(outputnode,user,outputname))
f.write("export OPENBLAS_NUM_THREADS=1\n")
f.write("{0}/{1}\n".format(pwd,execname))
f.close()
