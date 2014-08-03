APPNAME = 'etude'
VERSION = '0.0.0'

srcdir = '.'
blddir = 'build'

subdirs=''

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')

  opt.recurse(subdirs)

def configure(conf):
  conf.env.CXXFLAGS += ['-std=gnu++11', '-Og', '-Wall', '-Wextra', '-g']

  conf.load('compiler_cxx')
  conf.load('unittest_gtest')

  conf.recurse(subdirs)

def build(bld):
  bld.recurse(subdirs)
