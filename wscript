# -*- python -*-

APPNAME = 'etude'
VERSION = '0.0.0'

srcdir = '.'
blddir = 'build'

subdirs='etude tests'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')
  opt.load('gnu_dirs')

  opt.add_option('--enable-gcov',
                 dest='enable_gcov', action='store_true', default=False,
                 help='enable gcov to measure coverage')

  opt.recurse(subdirs)

def configure(conf):
  conf.env.CXXFLAGS += ['-std=gnu++11', '-Wall', '-Wextra', '-g']

  if conf.options.enable_gcov:
    conf.env.CXXFLAGS += ['-fprofile-arcs', '-ftest-coverage']
    conf.env.LINKFLAGS += ['-fprofile-arcs']

  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.load('gnu_dirs')

  if conf.check_cxx(cxxflags='-Og', mandatory=False):
    conf.env.CXXFLAGS += ['-Og']

  conf.recurse(subdirs)

def build(bld):
  bld(name='etude_headers', export_includes='./')
  bld.recurse(subdirs)
