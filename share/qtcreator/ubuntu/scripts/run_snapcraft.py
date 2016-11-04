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
import sys
import shutil
import subprocess
from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] lp:branch")
parser.add_option(
    "-s", "--snapcraft", dest="snapcraft")
options, args = parser.parse_args()

if options.snapcraft is None:
    options.snapcraft = shutil.which("snapcraft")

if options.snapcraft is None:
    parser.error("Snapcraft not found.")

if not os.path.isfile(options.snapcraft) or not os.access(options.snapcraft, os.X_OK):
    parser.error("-s must specify a executable file.")

print("Using snapcraft from :"+options.snapcraft)

sys.stdout.flush()
sys.stderr.flush()

ret = subprocess.call([options.snapcraft, "clean"])
if ret != 0:
    sys.exit(ret)

sys.exit(subprocess.call([options.snapcraft]+args))
