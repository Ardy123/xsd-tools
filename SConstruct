import BuildUtil
import os.path
import platform
import distro
import os

platforms = { 
    'Linux-Default': 'linux-default',
    'Linux-ubuntu' : 'linux-ubuntu',
    'ubuntu'       : 'linux-ubuntu',    
    'Darwin-darwin': 'darwin'
}

xsdb = {
    'src': [
        'src/main.cpp',
        'src/util.cpp',
        'src/luaScriptAdapter.cpp',
        'src/resource.cpp',
        'src/XSDParser/Exception.cpp',
        'src/XSDParser/Parser.cpp',
        'src/XSDParser/Types.cpp',
        'src/XSDParser/TypesDB.cpp',
        'src/XSDParser/Elements/Attribute.cpp',
        'src/XSDParser/Elements/Choice.cpp',
        'src/XSDParser/Elements/ComplexType.cpp',
        'src/XSDParser/Elements/Element.cpp',
        'src/XSDParser/Elements/List.cpp',
        'src/XSDParser/Elements/Node.cpp',
        'src/XSDParser/Elements/Restriction.cpp',
        'src/XSDParser/Elements/Schema.cpp',
        'src/XSDParser/Elements/Sequence.cpp',
        'src/XSDParser/Elements/SimpleType.cpp',
        'src/XSDParser/Elements/Union.cpp',
        'src/XSDParser/Elements/Group.cpp',
        'src/XSDParser/Elements/Any.cpp',
        'src/XSDParser/Elements/ComplexContent.cpp',
        'src/XSDParser/Elements/Extension.cpp',
        'src/XSDParser/Elements/SimpleContent.cpp',
        'src/XSDParser/Elements/MinExclusive.cpp',
        'src/XSDParser/Elements/MaxExclusive.cpp',
        'src/XSDParser/Elements/MinInclusive.cpp',
        'src/XSDParser/Elements/MaxInclusive.cpp',
        'src/XSDParser/Elements/MinLength.cpp',
        'src/XSDParser/Elements/MaxLength.cpp',
        'src/XSDParser/Elements/Length.cpp',
        'src/XSDParser/Elements/FractionDigits.cpp',
        'src/XSDParser/Elements/Pattern.cpp',
        'src/XSDParser/Elements/TotalDigits.cpp',
        'src/XSDParser/Elements/WhiteSpace.cpp',
        'src/XSDParser/Elements/Enumeration.cpp',
        'src/XSDParser/Elements/AttributeGroup.cpp',
        'src/XSDParser/Elements/Include.cpp',
        'src/XSDParser/Elements/Annotation.cpp',
        'src/XSDParser/Elements/Documentation.cpp',
        'src/XSDParser/Elements/All.cpp',
        'src/XSDParser/Elements/AppInfo.cpp',
        'src/Processors/ArrayType.cpp',
        'src/Processors/LuaProcessorBase.cpp',
        'src/Processors/LuaAdapter.cpp',
        'src/Processors/LuaProcessor.cpp',
        'src/Processors/SimpleTypeExtracter.cpp',
        'src/Processors/ElementExtracter.cpp',
        'src/Processors/RestrictionVerify.cpp',
        'src/TemplateEngine/table.lua',
        'src/TemplateEngine/string.lua',
        'src/TemplateEngine/parser.lua',
        'src/TemplateEngine/safeEnv.lua',
        'src/TemplateEngine/stringbuffer.lua',
        'src/TemplateEngine/templateEngine.lua'],
    'libs': {
        'linux-default' : [ 'tinyxml', 'libboost_system',
                            'boost_filesystem', 'lua', 'incbin' ],
        'linux-ubuntu' : ['tinyxml', 'libboost_system',
                          'boost_filesystem', 'lua5.1', 'incbin' ],
        'darwin' : ['tinyxml', 'libboost_system', 'boost_filesystem',
                    'lua5.1', 'incbin' ] 
    },
    'cflags': {
        'linux-default' : {
            'debug'  : '-Wall -Werror -I. -g -Winit-self -Wformat ' \
            '-Wformat-nonliteral ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral',
            'release': '-Wall -Werror -I. -O3 -fomit-frame-pointer ' \
            '-Winit-self -Wformat -Wformat-nonliteral ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral'
        },
        'linux-ubuntu' : {
            'debug'  : '-Wall -Werror -I. -g -Winit-self -Wformat ' \
            '-Wformat-nonliteral ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral ' \
            '-no-pie',
            'release': '-Wall -Werror -I. -O3 -fomit-frame-pointer ' \
            '-ffunction-sections -fdata-sections -Winit-self ' \
            '-Wformat -Wformat-nonliteral ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral -no-pie'
            },
        'darwin' : {
            'debug'  : '-std=c++11 -Wall -Werror -I. -g -Winit-self ' \
            '-Wformat -Wformat-nonliteral -I/usr/local/include ' \
            '-Wno-unused-local-typedefs ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral',
            'release': '-std=c++11 -Wall -Werror -I. -O3 ' \
            '-fomit-frame-pointer -Winit-self -Wformat ' \
            '-Wformat-nonliteral -I/usr/local/include ' \
            '-Wno-unused-local-typedefs ' \
            '-Wno-potentially-evaluated-expression ' \
            '-Wno-format-nonliteral'
        }
    },
    'luaflags': {
        'debug': '', 
        'release': '-s' 
    },
    'linkflags': {
        'linux-default' : {
            'debug':'-g',
            'release': '-Wl,-O1 -Wl,--discard-all -Wl,--no-undefined -Wl,--gc-sections'
        },
        'linux-ubuntu' : {
            'debug':'-g -no-pie',
            'release': '-Wl,-O1 -Wl,--discard-all -Wl,--no-undefined ' \
            '-no-pie -Wl,--gc-sections'
        },
        'darwin' : {
            'debug':'-g',
            'release': '',
        }
    },
    'target': 'xsdb'
}

# Get arguments
default_build_platform = platforms.get(
    ("%s-%s" % (platform.system(), distro.id())),
    'Linux-Default')
release_target      = ARGUMENTS.get('conf', 'release')
install_prefix      = ARGUMENTS.get('prefix', '/usr/')
target_platform     = ARGUMENTS.get('target', default_build_platform)
install_target_bin  = os.path.join(install_prefix, 'bin/')
install_target_data = os.path.join(install_prefix,
                                   'share/xsdtools/templates/')

# Setup Environment
env_setup = {
    'CC'    : os.environ.get('CC', 'gcc'),
    'CXX'   : os.environ.get('CXX', 'g++'),
    'LD'    : os.environ.get('LD', 'ld'),
    'AR'    : os.environ.get('AR', 'ar'),
    'STRIP' : os.environ.get('STRIP', 'strip')
}
env = BuildUtil.SetupEnv(xsdb,
                         Environment(**env_setup),
                         target_platform,
                         release_target)

# Build xsdb
lua, xsdb = BuildUtil.Program(xsdb, env, release_target)

# Install xsdb
installxsdb = env.Install(install_target_bin, xsdb)
installData = env.Command(install_target_data, 
                          'templates/', 
                          Copy("$TARGET", "$SOURCE"))
env.Alias('install', [installxsdb, installData])

# Uninstall xsdb
env.Command('uninstall', 
            None, 
            [Delete(installxsdb),Delete(installData)])

# Default build
Default(lua, xsdb)
