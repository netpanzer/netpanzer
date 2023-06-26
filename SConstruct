import os
import glob
import sys
import string
import subprocess

################################################################
# Set NetPanzer Version
################################################################

NPVERSION = '0.9.0-RC4'
SVERSION = ''

try:
    FILE = open('RELEASE_VERSION', 'r')
    NPVERSION = FILE.readline().strip(' \n\r')
    FILE.close()
except:
    pass

try:
    SVERSION = os.popen('svnversion').read()[:-1]
    SVERSION = SVERSION.split(':')[-1]
except:
    pass

print("NPVERSION = " + NPVERSION)
print("SVERSION = " + SVERSION)
if NPVERSION == '' and SVERSION != '':
    NPVERSION = 'svn-' + SVERSION;

thisplatform = sys.platform;
print('Building version ' + NPVERSION + ' in ' + thisplatform)

################################################################
# Fix compiling with long lines in windows
################################################################
class ourSpawn:
    def ourspawn(self, sh, escape, cmd, args, env):
        newargs = ' '.join(args[1:])
        cmdline = cmd + " " + newargs
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err)
            print("=====")
        return rv

def SetupSpawn( env ):
    if sys.platform == 'win32':
        buf = ourSpawn()
        buf.ourenv = env
        env['SPAWN'] = buf.ourspawn

################################################################
# Modified copy from BosWars of globSources
################################################################

def globSources(localenv, sourcePrefix, sourceDirs, pattern):
    sources = []
    sourceDirs = Split(sourceDirs)
    for d in sourceDirs:
        sources.append(glob.glob( sourcePrefix + '/' + d + '/' + pattern))
    sources = Flatten(sources)
    targetsources = []
    for s in sources:
        targetsources.append(buildpath + s)
    return targetsources

################################################################
# Make static libraries
################################################################

def MakeStaticLib(localenv, libname, libdirs, pattern):
    sources = globSources(localenv, 'src/Lib', libdirs, pattern)
    localenv.StaticLibrary( libpath + libname, sources)

################################################################
# Add options
################################################################

opts = Variables()
opts.AddVariables(
    EnumVariable('mode', 'set compile mode', 'release', allowed_values=('debug', 'release')),
    EnumVariable('cross','do a cross compilation','', allowed_values=('','mingw','linux')),
    ('datadir','define the extra directory where the netpanzer will look for data files, usefull for linux distributions, defaults to no extra directory',''),
    ('sdlconfig','sets the sdl-config full path, cross compilation sure needs this', 'sdl2-config'),
    ('universal','builds universal app in Max OS X(default false, other value is true)', 'false'),
    ('compilerprefix', 'sets the prefix for the cross linux compiler, example: i686-pc-linux-gnu-', ''),
)

env = Environment(ENV = os.environ, options = opts)
Help(opts.GenerateHelpText(env))

env.Append( CCFLAGS = [ '-DPACKAGE_VERSION=\\"' + NPVERSION + '\\"' ] )

if env['datadir'] != '':
    env.Append( CCFLAGS = [ '-DNP_DATADIR=\\"' +  env['datadir'] + '\\"' ])

buildpath = 'build/'
if env['cross'] != '':
    buildpath += env['cross'] + '/'

buildpath += env['mode'] + '/'

libpath = buildpath + 'libs/'

if env['cross'] != '':
    binpath = buildpath
else:
    binpath = './'

exeappend = ''

if 'CXXFLAGS' in os.environ:
    env.Append(CCFLAGS = os.environ['CXXFLAGS'])

if 'CPPFLAGS' in os.environ:
    env.Append(CCFLAGS = os.environ['CPPFLAGS'])

if 'LDFLAGS' in os.environ:
    env.Append(LINKFLAGS = os.environ['LDFLAGS'])

if env['cross'] == 'mingw':
    print('configuring for mingw cross compilation')
    env.Tool('crossmingw', toolpath = ['.'])
    env.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    env.Append( LDFLAGS = [ '-mwindows' ] )
    env.Append( LIBS = [ 'ws2_32', 'mingw32' ] )
    env['WINICON'] = env.RES( 'support/icon/npicon.rc' )


env.Append( LINKFLAGS = [ '-static-libgcc' ] )

env.Append(CCFLAGS = '-std=c++17')

if env['mode'] == 'debug':
    env.Append(CCFLAGS = ['-g', '-O0'])
    #exeappend = '-debug'
else:
    if env['cross'] == 'mingw':
        # it seems -O2 makes the mingw port to crash, use -O1
        env.Append(CCFLAGS = '-O1')
    else:
        env.Append(CCFLAGS = '-O2')
        env.Append(CCFLAGS = '-s')
        # if needed to built static...
        #env.Append( LINKFLAGS = [ '-static-libgcc', '-static-libstdc++' ] )
env.Append(CCFLAGS = ['-Wall'])

# keep for reference for when redoing the linux cross compilation
#    crosslinuxenv= Environment(ENV = {'PATH' : os.environ['PATH']},tools = ['gcc','g++','ar','gnulink'], options=opts )
#    crosslinuxenv.Replace( CXX = env['crosslinuxcompilerprefix'] + env['CXX'] )
#    crosslinuxenv.Replace( CC = env['crosslinuxcompilerprefix'] + env['CC'] )
#    crosslinuxenv.Replace( AR = env['crosslinuxcompilerprefix'] + env['AR'] )
#    crosslinuxenv.Replace( RANLIB = env['crosslinuxcompilerprefix'] + env['RANLIB'] )
#    crosslinuxenv.Append( LINKFLAGS = [ '-static-libgcc' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '-static-libgcc' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '-lstdc++' ] )
#    #crosslinuxenv.Append( _LIBFLAGS = [ '`' + crosslinuxenv['CXX'] + ' -print-file-name=libstdc++.a`' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '/usr/local/gcc/i686-linux/lib/gcc/i686-linux/4.2.4/../../../../i686-linux/lib/libstdc++.a' ] )


env.VariantDir(buildpath,'.',duplicate=0)

luaenv = env.Clone()
physfsenv = env.Clone()
networkenv = env.Clone()

################################################################
# Configure Environments
################################################################

env.Append( CPPPATH = [ 'src/Lib', 'src/NetPanzer', 'src/Lib/physfs', 'src/Lib/lua'] )

# for this platform
if thisplatform == 'darwin':
    env.Append( CPPPATH = ['/Library/Frameworks/SDL2.framework/Headers',
                           '/Library/Frameworks/SDL2_mixer.framework/Headers' ] )
    networkenv.Append( CPPPATH = ['/Library/Frameworks/SDL2.framework/Headers'] )
    if env['universal'] != 'false':
        env.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
        luaenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
        physfsenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
        networkenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
        env.Append( LINKFLAGS = [ '-mmacosx-version-min=10.4', '-arch', 'ppc', '-arch', 'i386' ] )
    else:
        env.Append( CCFLAGS = [ '-arch', 'i386' ] )
        luaenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        physfsenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        networkenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        env.Append( LINKFLAGS = [ '-arch', 'i386' ] )
    env.AppendUnique(FRAMEWORKS=Split('SDL2 SDL2_mixer Cocoa IOKit'))
    env.Append( NPSOURCES =  ['support/macosx/SDLMain.m'] )
elif thisplatform == 'win32':
    env.Tool('mingw', toolpath = ['.']) # we assume gcc
    env.Append( CPPPATH = [ 'C:/MinGW/include/SDL2' ] )
    networkenv.Append( CPPPATH = [ 'C:/MinGW/include/SDL2' ] )
    #env.Append( LIBPATH = [ 'C:/MinGW/lib' ] )
    env.Append( LIBS = [ 'ws2_32', 'mingw32', 'SDLMain', 'SDL2' ] )
    env.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    networkenv.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    env.Append( _LIBFLAGS = [ '-mwindows' ] )
#     env.Prepend( _LIBFLAGS = [ 'C:/MinGW/lib/SDL2_mixer' ] )
    env['WINICON'] = env.RES( 'support/icon/npicon.rc' )
    SetupSpawn(env)
else:
    networkenv.ParseConfig(env['sdlconfig'] + ' --cflags --libs')
    env.ParseConfig(env['sdlconfig'] + ' --cflags --libs')
    # note: for some magic reason, now doesn't need to put the full path for
    # SDL_mixer when using mingw crosscompilation
    env.Append( LIBS = ['SDL2', 'SDL2_mixer', 'SDL2_ttf' ] )

################################################################
# Makes libs
################################################################

# BUILDS NETWORK
networkenv.Append(           CPPPATH = [ 'src/Lib' ] )
MakeStaticLib(          networkenv, 'npnetwork', 'Network', '*.cpp')

# BUILDS LUA
luaenv.Append(           CPPPATH = [ 'src/Lib/lua'] )
MakeStaticLib(          luaenv, 'nplua', 'lua', '*.c')

# BUILDS PHYSFS
physfsenv.Append( CFLAGS = [ '-DPHYSFS_SUPPORTS_ZIP=1', '-DZ_PREFIX=1', '-DPHYSFS_NO_CDROM_SUPPORT=1' ] )
physfsenv.Append( CPPPATH = [ 'src/Lib/physfs', 'src/Lib/physfs/zlib123' ] )
MakeStaticLib(physfsenv, 'npphysfs', 'physfs physfs/platform physfs/archivers physfs/zlib123', '*.c')

# BUILDS 2D
env.Append( CFLAGS = [ '-DZ_PREFIX=1' ] )
env.Append( CPPPATH = 'src/Lib/physfs/zlib123' )
MakeStaticLib(env, 'np2d', '2D 2D/libpng', '*.c*')

# BUILDS REST OF LIBRARIES
MakeStaticLib(env, 'nplibs', 'ArrayUtil INIParser Types Util optionmm','*.cpp')

################################################################
# NetPanzer source dirs
################################################################

npdirs = """
    Bot Classes Classes/AI Classes/Network Core Interfaces Network
    Objectives Particles PowerUps Resources Scripts System Units Weapons
    Views Views/Components Views/Game Views/MainMenu Views/MainMenu/Multi
    Views/MainMenu/Multi/MasterServer Views/MainMenu/Options
"""

env.Append( NPSOURCES = globSources(env, 'src/NetPanzer', npdirs, "*.cpp") )
if 'WINICON' in env:
    env.Append( NPSOURCES = env['WINICON'] )

env.Prepend( LIBS = ['np2d','nplua','npnetwork','nplibs','npphysfs'] )
env.Prepend( LIBPATH = libpath )

netpanzer = env.Program( binpath+'netpanzer'+exeappend, env['NPSOURCES'])

pak2bmp = env.Program( binpath+'pak2bmp'+exeappend, 'support/tools/pak2bmp.cpp')
Alias('pak2bmp',pak2bmp)

bmp2pak = env.Program( binpath+'bmp2pak'+exeappend, 'support/tools/bmp2pak.cpp')
Alias('bmp2pak',bmp2pak)

Default(netpanzer)
