#!/usr/bin/env python2

##############################################################################
# Copyright (c) 2012, GeoData Institute (www.geodata.soton.ac.uk)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  - Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#  - Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
##############################################################################

"""
Output mapserver configuration information to `node-gyp`

Configuration options are retrieved from environment variables set using `npm
config set`.  This allows for a simple `npm install mapserv` to work.
"""

from optparse import OptionParser
import os

def get_lib_dir():
    return os.environ.get('npm_config_mapserver_lib_dir', '')

parser = OptionParser()
parser.add_option("--include",
                  action="store_true", default=False,
                  help="output the mapserver include path")

parser.add_option("--ldflags",
                  action="store_true", default=False,
                  help="output the mapserver library rpath option")

(options, args) = parser.parse_args()

if options.include:
    print os.environ.get('npm_config_mapserver_include_dir', '')

if options.ldflags:
    # write the library path into the resulting binary
    lib_dir = get_lib_dir()
    if lib_dir:
        print "-Wl,-rpath=%s -L%s" % (lib_dir, lib_dir)
    print '-Wl,--no-as-needed,-lmapserver'