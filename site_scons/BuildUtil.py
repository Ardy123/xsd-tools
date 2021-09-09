# Copyright: (c)2012 Ardavon Falls
#
# This file is part of xsd-tools.
#
# xsd-tools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# xsd-tools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import re
import SCons.Util

CSuffixes = ['.c', '.m']
CXXSuffixes = ['.cpp', '.cc', '.cxx', '.c++', '.C++', '.mm']
LUASuffixes = ['.lua', '.luac']


def _extractCPPFiles(buildSettings):
    srcLSt = buildSettings['src']
    retLst = []
    for suffix in CSuffixes:
        retLst.extend([x for x in srcLSt if os.path.splitext(x)[1] == suffix])
    for suffix in CXXSuffixes:
        retLst.extend([x for x in srcLSt if os.path.splitext(x)[1] == suffix])
    return retLst

def _extractLuaFiles(buildSettings):
    srcLSt = buildSettings['src']
    retLst = []
    for suffix in LUASuffixes:
        retLst.extend([x for x in srcLSt if os.path.splitext(x)[1] == suffix])
    return retLst

def _sconsWrap(str):
    return SCons.Util.CLVar(str)

def SetupEnv(buildSettings, env, platform, config):
    env['ENV']['PATH'] = os.environ['PATH']
    # setup lua builders
    env.Tool('luac')
    # pull compiler from environment variables
    env['CC'] = SCons.Util.CLVar(os.environ.get('CC', str(env['CC'])))
    # set C/C++/Lua/link flags with variant settings
    env['CFLAGS']   = SCons.Util.CLVar(buildSettings['cflags'][platform][config])
    env['CCFLAGS']  = SCons.Util.CLVar(buildSettings['cflags'][platform][config])
    env['LUACFLAGS']= SCons.Util.CLVar(buildSettings['luaflags'][config])
    env['LINKFLAGS']= SCons.Util.CLVar(buildSettings['linkflags'][platform][config])
    # update C/C++ flags with paths
    for lib in buildSettings['libs'][platform]:
        try:
            env.ParseConfig('PKG_CONFIG_PATH=. pkg-config --silence-errors --cflags --libs ' + lib)
        except OSError:
            env.MergeFlags(('-l%s' % (lib)))
    return env

def Program(buildSettings, env, config):
    lua = env.Lua('luascript', _extractLuaFiles(buildSettings))

    xsdb = None
    targets  = buildSettings['target']
    cppFiles = _extractCPPFiles(buildSettings) 
    if sys.platform.startswith("linux"):
        xsdb = env.Program(targets, cppFiles + lua)
    elif sys.platform.startswith("darwin"):
        xsdb = env.Program(targets, cppFiles)

    return lua, xsdb
