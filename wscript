import Options
from os import unlink, symlink, popen
from os.path import exists 

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_tool('osx')


def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "mapserver_bindings"
  obj.source = "src/mapserver_bindings.cc"

def shutdown():
  if Options.commands['clean']:
    if exists('mapserver_binding.node'): unlink('mapserver_binding.node')
  else:
    if exists('build/default/mapserver_binding.node') and not exists('mapserver_binding.node'):
      symlink('build/default/mapserver_binding.node', 'mapserver_binding.node')
