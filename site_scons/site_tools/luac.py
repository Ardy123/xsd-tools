# -*- coding: utf-8 -*-
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

import SCons.Builder
import SCons.Tool
import SCons.Defaults

LuacObjectBuilder = SCons.Builder.Builder(action = '$LUACMD', suffix = '.luac', src_suffix = '.lua')
LuacProgramBuilder = SCons.Builder.Builder(action = '$LLCCMD', suffix = '.luac', src_suffix = '.luac', src_builder = ['LuaC'])

def generate(env):
	# add generic object builders for 'program' builder
	staticObjectBuilder, sharedObjectBuilder = SCons.Tool.createObjBuilders(env)
	staticObjectBuilder.add_action('.luac', '$LUAOBJCMD')
	sharedObjectBuilder.add_action('.luac', '$LUAOBJCMD')
	# set env variables/settings
	env['BUILDERS']['LuaC'] = LuacObjectBuilder
	env['LUAC'] = 'luac'
	env['LUACFLAGS'] = ''
	env['LUACMD'] = '$LUAC -o $TARGET $LUACFLAGS $SOURCE'
  	env['BUILDERS']['Lua'] = LuacProgramBuilder
	env['LLCCMD'] = '$LUAC -o $TARGET $LUACFLAGS $SOURCES'
	env['OBJCPY'] = 'objcopy'
	env['LUAOBJCMD'] = '$OBJCPY --input binary --output elf64-x86-64 --binary-architecture i386 $SOURCE $TARGET'

	
def exists(env):
	return env.Detect('luac') and env.Detect('objcopy')