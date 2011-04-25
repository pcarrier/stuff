APPNAME='stuff'
VERSION='0.1'

portable_bins = [
  'auth/grouplist',
  'fun/b2c',
  'fun/mkpasswd',
  'fun/nato',
  'fun/superglob',
  'mem/eatmemory',
  'mem/mmapdoublecheck',
  'mem/mmapnwait',
  'sys/sethostid'
]

preload_libs = [
  'diagnostics/gdb4undeads',
  'diagnostics/memcpy2memmove',
  'diagnostics/mtrace'
]

linux_bins = [
  'fs/wtfitf',
  'mem/hugepagesdoublecheck',
  'mem/hugepagesmaxalloc'
]

test_bins = [
  'fun/async/test-poll',
  'fun/async/test-select',
  'sys/i_segv',
  'sys/i_segv2'
]

test_pthread_bins = [
  'sys/i_segv3'
]

mini_bins = [
  'mini/echo',
  'mini/false',
  'mini/hostid',
  'mini/logname',
  'mini/sync',
  'mini/true',
  'mini/yes',
  'mini/yes2'
]

glib_bins = [
  'sys/errnos'
]

from waflib.Task import Task

def options(opt):
  opt.load('compiler_c')

def configure(cnf):
  cnf.load('compiler_c')
  cnf.check(header_name = 'linux/fiemap.h', mandatory=False) #features='c cprogram', mandatory=False)
  cnf.check_cc(cflags=['-Wall','-Wextra', '-pedantic', '-std=c99'], defines=['_XOPEN_SOURCE'], uselib_store='base')
  cnf.check_cc(lib=['m'], uselib_store='m')
  cnf.check_cc(lib=['pthread'], uselib_store='pthread')
  cnf.check_cc(cflags=['-Werror'], uselib_store='strict')
  cnf.check_cfg(package='glib-2.0', args=['--cflags', '--libs'], use='portable', uselib_store='glib2')
  cnf.write_config_header('config.h')

def build(bld):
  bld(rule=bld.path.abspath()+'/sys/errnos.h.gen ${SRC} ${TGT} ${ROOT}', source='sys/errnos.list', target='errnos.h')
  for bin in portable_bins+mini_bins+test_bins:
    bld.program(source=bin+'.c', target=bin, use=['base', 'strict', 'm'])
  for bin in linux_bins:
    bld.program(source=bin+'.c', target=bin, use=['base', 'strict', 'm'])
  for lib in preload_libs:
    # I stopped trying to be strict here...
    bld.shlib(source=lib+'.c', target=lib, use=['base', 'm'])
  for bin in test_pthread_bins:
    bld.program(source=bin+'.c', target=bin, use=['base', 'strict', 'pthread'])