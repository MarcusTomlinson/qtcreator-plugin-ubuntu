#!/usr/bin/python3

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

ret = subprocess.call([options.snapcraft, "clean"])
if ret != 0:
    sys.exit(ret)

sys.exit(subprocess.call([options.snapcraft]+args))
