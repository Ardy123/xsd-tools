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

def _createIncludePaths(libs):
    flags=''
    for lib in libs:
        cflag = os.popen('pkg-config --silence-errors --cflags '+lib)
        flags += cflag.readline()
    return ' ' + flags.replace('\n','')

def _createLibDeps(libs):
    flags=''
    for lib in libs:
        ldflag = os.popen('pkg-config --silence-errors --libs '+lib)
        tmpflag = ldflag.readline()
        if ldflag.close() == 0:
            flags += tmpflag
        else:
            flags += '-l' + lib+' '
    return flags.replace('\n','').replace('-l','').split()

def _createCFlags(buildSettings, config):
    return buildSettings["cflags"][config] + \
           _createIncludePaths(buildSettings['libs'])

def _createCScannerPaths(buildSettings, config):
    return re.findall('-I(\S+)', _createCFlags(buildSettings, config))
    
def _createLuaFlags(buildSettings, config):
    return buildSettings["luaflags"][config]

def _createLdFlags(buildSettings, config):
    return buildSettings["linkFlags"][config]

def _extractTarget(buildSettings):
    return buildSettings['target']

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

def SetupEnv(buildSettings, env, config):
    # setup lua builders
    env.Tool('luac')
    # setup C flags and lib paths
    if os.environ.has_key('CC'):
        env['CC'] = os.environ['CC']
    env['CCFLAGS'] = _sconsWrap(_createCFlags(buildSettings, config))
    env['LIBS'] = _sconsWrap(_createLibDeps(buildSettings['libs']))
    env['LIBPATH'] = _sconsWrap(buildSettings['libpath'] if 'libpath' in buildSettings else "")
    env['CPPPATH'] = _sconsWrap(_createCScannerPaths(buildSettings, config))
    env['LUACFLAGS']= _sconsWrap(_createLuaFlags(buildSettings, config))
    if sys.platform.startswith("linux"):
        env['LINKFLAGS']= _sconsWrap(_createLdFlags(buildSettings, config))
    elif sys.platform == "darwin":
        linkData = ' -sectcreate __DATA __luascript_luac luascript.luac '
        buildSettings['linkFlags']={'debug':'-g'+linkData,'release':'-Wl,-S'+linkData} 
        buildSettings['cflags']['release']+='-Wl,-S'
        env['LINKFLAGS']= _sconsWrap(_createLdFlags(buildSettings, config))
    return env

def Program(buildSettings, env, config):
    luaDeps = _sconsWrap("")
    if sys.platform.startswith("linux"):
        luaDeps = env.Lua('luascript', _extractLuaFiles(buildSettings))
    return env.Program(_extractTarget(buildSettings), _extractCPPFiles(buildSettings) + luaDeps)
