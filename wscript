# vim: ft=python
from sys import platform

APPNAME='stuff'
VERSION='0.1'

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')
    # conf.check_cc(header_name = 'linux/fiemap.h', mandatory=False)
    conf.check_cc(function_name='fgetgrent', header_name="grp.h", mandatory=False)
    conf.check_cc(cflags=['-Wall','-Wextra', '-pedantic', '-std=c99'],
        defines=['_XOPEN_SOURCE=500'], uselib_store='base')
    conf.check_cc(lib=['m'], uselib_store='m')
    conf.check_cc(lib=['dl'], defines=['_GNU_SOURCE'], uselib_store='dl')
    conf.check_cc(lib=['pthread'], uselib_store='pthread')
    conf.check_cc(cflags=['-Werror'], uselib_store='strict')
    conf.check_cfg(package='glib-2.0', args=['--cflags', '--libs'],
        use='portable', uselib_store='glib2')

common_use=['base', 'strict', 'm']

def build(build):
    build(rule=build.path.abspath()+'/sys/errnos.h.gen ${SRC} ${TGT}',
        source='sys/errnos.list', target='../sys/errnos.h')

    # The basics, should be on any recent Unix system, and we're strict :)
    for bin in ['fun/b2c', 'fun/mkpasswd', 'fun/nato', 'fun/superglob',
        'sys/sethostid', 'auth/grouplist']:
        build.program(source=bin+'.c', target=bin, use=common_use)

    # mini stuff, shouldn't invade your PATH ever unless you're completely mad
    for bin in ['mini/echo', 'mini/false', 'mini/hostid', 'mini/logname',
        'mini/sync', 'mini/true', 'mini/yes', 'mini/yes2']:
        build.program(source=bin+'.c', target=bin, use=common_use,
            install_path = "${PREFIX}/bin/mini")

    # Linux-specific stuff
    if platform is 'linux':
        for bin in ['fs/wtfitf', 'mem/hugepagesdoublecheck',
            'mem/hugepagesmaxalloc']:
            build.program(source=bin+'.c', target=bin, use=common_use)

    # LD_PRELOAD libs
    ld_libs = ['diagnostics/gdb4undeads', 'diagnostics/sigomgbt']

    if platform is 'linux':
        ld_libs += [ 'diagnostics/mtrace', 'diagnostics/memcpy2memmove' ]

    for lib in ld_libs:
        # I stopped trying to be strict here...
        build.shlib(source=lib+'.c', target=lib, use=['base', 'm', 'dl'])

    # pthread stuff
    for bin in ['sys/i_segv3']:
        build.program(source=bin+'.c', target=bin, use=['base', 'strict', 'pthread'])

    # glib stuff
    for bin in ['sys/errnos']:
        build.program(source=bin+'.c', target=bin, use=['base', 'strict', 'glib2'])

    # Cool 7-segment stuff
    for bin in ['fun/7seg/7plot', 'fun/7seg/7clock']:
        build.program(source=[bin+'.c','fun/7seg/7seg.c'], target=bin, use=common_use)

    # Test stuff (not installed)
    for bin in ['fun/async/test-poll', 'fun/async/test-select',
        'sys/i_segv', 'sys/i_segv2', 'mem/eatmemory',
        'mem/mmapdoublecheck', 'mem/mmapnwait']:
        build.program(source=bin+'.c', target=bin, use=common_use, install_path=None)

    # Python scripts
    build.install_files('${PREFIX}/bin', 'auth/pinlock net/bwstats net/throttler fun/slider', chmod=0755)
