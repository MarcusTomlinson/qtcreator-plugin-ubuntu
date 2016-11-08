#!/usr/bin/python3
# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# QTC device applauncher
# Copyright (C) 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Benjamin Zeller <benjamin.zeller@canonical.com>

import os
import os.path
import subprocess
import argparse
import signal
import sys

# register options to the argument parser
parser = argparse.ArgumentParser(description="SDK snap launcher")
parser.add_argument('snap_package',action="store")
parser.add_argument('snap_command',action='store')
parser.add_argument('--devmode'   ,action='store_true')

options, args = parser.parse_known_args()

if not os.path.isfile(options.snap_package):
    print("Snap file does not exist")
    sys.exit(1)

#parse the package name which should be  application_version_arch.snap
app_info = os.path.basename(options.snap_package)[:-5].split("_")
if len(app_info) != 3:
    print("The snap package name is invalid", file=sys.stderr)
    sys.exit(1)

#flush all descriptors
sys.stdout.flush()
sys.stderr.flush()

#ok lets install the snap package
snaparg = ["snap", "install", options.snap_package, "--dangerous"]
if options.devmode:
    print("Enabling devmode\n")
    snaparg += ["--devmode"]
ret = subprocess.call(snaparg, stdout=sys.stdout, stderr=sys.stderr)
if ret != 0:
    sys.exit(ret)

#flush all descriptors
sys.stdout.flush()
sys.stderr.flush()

#this will contain the actual subprocess object
proc = None
stopped = False
def signalHandler(signum, frame):
    print('Signal handler called with signal', signum)
    if proc is not None:
        proc.kill()
    else:
        stopped = True

signal.signal(signal.SIGINT,  signalHandler)
signal.signal(signal.SIGTERM, signalHandler)
signal.signal(signal.SIGHUP,  signalHandler)

#start the actual app
print("Command args are: "+str(args)+"\n")
if not stopped:
    proc = subprocess.Popen(["snap", "run", app_info[0]+"."+options.snap_command]+args, stdout=sys.stdout, stderr=sys.stderr)
    ret = proc.wait()

#flush all descriptors
sys.stdout.flush()
sys.stderr.flush()

subprocess.call(["snap", "remove", app_info[0]], stdout=sys.stdout, stderr=sys.stderr)

sys.exit(ret)
