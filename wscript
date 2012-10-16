# vim: ft=python

from waflib import Logs
from waflib.Build import BuildContext

APPNAME = 'stuff'
VERSION = '0.1'


def options(opt):
    opt.load('compiler_c')


def configure(conf):
    conf.load('compiler_c')
    conf.check_cc(function_name='fgetgrent',
                  header_name="grp.h",
                  mandatory=False)
    conf.check_cc(header_name="X11/Xlib.h", mandatory=False)
    conf.check_cc(cflags=['-Wall', '-Wextra', '-pedantic', '-std=c99'],
                  defines=['_XOPEN_SOURCE=700'],
                  uselib_store='base')
    conf.check_cc(lib=['m'],
                  uselib_store='m')
    conf.check_cc(lib=['dl'],
                  defines=['_GNU_SOURCE'],
                  uselib_store='dl')
    conf.check_cc(lib=['crypt'],
                  uselib_store='crypt',
                  mandatory=False)
    conf.check_cc(lib=['pthread'],
                  uselib_store='pthread')
    conf.check_cc(cflags=['-Werror'],
                  uselib_store='strict')
    conf.check_cfg(package='glib-2.0',
                   args=['--cflags', '--libs'],
                   use='portable',
                   uselib_store='glib2',
                   mandatory=False)
    conf.check_cfg(package='libnetfilter_conntrack',
                   args=['--cflags', '--libs'],
                   use='portable',
                   uselib_store='nfconntrack',
                   mandatory=False)
    if conf.env.DEST_OS is 'linux':
        conf.env.LINUX = True
    summary(conf)


class Summary(BuildContext):
    cmd = 'summary'
    fun = 'summary'


def summary(smr):
    def report_if_missing(var, name):
        if not var:
            Logs.error('Binaries requiring {0} won\'t be compiled!'.format(name))
        return var

    report_if_missing(smr.env.LIB_glib2,
                      'glib2')
    report_if_missing(smr.env.LIB_nfconntrack,
                      'libnetfilter_conntrack')
    report_if_missing(smr.env.LINUX,
                      'Linux')


STANDARD_USE = ['base', 'strict', 'm']


def build(build):
    build(rule=build.path.abspath() + '/sys/errnos.h.gen ${SRC} ${TGT}',
          source='sys/errnos.list',
          target='../sys/errnos.h')

    # The basics, should be on any recent Unix system, and we're strict :)
    for bin in ['fun/b2c', 'fun/mkpasswd', 'fun/firemeplz', 'fun/nato', 'fun/superglob',
        'sys/sethostid', 'auth/grouplist', 'fun/forking', 'fun/ip2hex', 'fun/hex2ip',
        'fun/hello_world', 'crap/xchathash']:
        build.program(source=bin + '.c',
                      target=bin,
                      use=STANDARD_USE)

    for bin in ['sys/crypt']:
        build.program(source=bin + '.c',
                      target=bin,
                      use=STANDARD_USE) # + ['crypt']) under linux TODO

    # mini stuff, shouldn't invade your PATH ever unless you're completely mad
    for bin in ['mini/echo', 'mini/false', 'mini/hostid', 'mini/logname',
        'mini/sync', 'mini/true', 'mini/yes', 'mini/yes2']:
        build.program(source=bin + '.c',
                     target=bin,
                     use=STANDARD_USE,
                     install_path="${PREFIX}/bin/mini")

    # Linux-specific stuff
    if build.env.LINUX:
        for bin in ['fs/wtfitf', 'mem/hugepagesdoublecheck',
            'mem/hugepagesmaxalloc', 'sys/leap_set', 'sys/leap_get']:
            build.program(source=bin + '.c',
                          target=bin, use=STANDARD_USE)

    if build.env.LIB_nfconntrack:
        for nf_bin in ['sys/conntail']:
            build.program(source='sys/conntail.c',
                          target='sys/conntail',
                          use=['base', 'nfconntrack'])

    # LD_PRELOAD libs
    ld_libs = ['diagnostics/gdb4undeads', 'diagnostics/sigomgbt']
    if build.env.LINUX:
        ld_libs.extend(['diagnostics/mtrace',  'diagnostics/memcpy2memmove'])
# 'adm/dellkeycodes': only if Xlib is available TODO

    for lib in ld_libs:
        # I stopped trying to be strict here...
        build.shlib(source=lib + '.c',
                    target=lib,
                    use=['base', 'm', 'dl'])

    # Cool 7-segment stuff
    for bin in ['fun/7seg/7plot', 'fun/7seg/7clock']:
        build.program(source=[bin + '.c', 'fun/7seg/7seg.c'],
                      target=bin,
                      use=STANDARD_USE)

    if build.env.LIB_glib2:
        for bin in ['sys/errnos', 'fun/urlencode', 'fun/urldecode']:
            build.program(source=bin + '.c',
                          target=bin, includes='.',
                          use=['base', 'strict', 'glib2'])

    build.program(source='sys/i_segv3.c',
                  target='sys/i_segv3',
                  use=['base', 'strict', 'pthread'])

    # Test stuff (not installed)
    for bin in ['fun/async/test-poll', 'fun/async/test-select',
        'sys/i_segv', 'sys/i_segv2', 'mem/eatmemory',
        'mem/mmapdoublecheck', 'mem/mmapnwait']:
        build.program(source=bin + '.c',
                      target=bin,
                      use=STANDARD_USE,
                      install_path=None)

    # Python scripts
    # build.install_files('${PREFIX}/bin',
    #                     'auth/pinlock net/bwstats net/throttler fun/slider',
    #                     chmod=0755)
