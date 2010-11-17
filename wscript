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
  # to build on linux comment following line
  conf.check_tool('osx')


def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "mapserver_bindings"
  obj.source = "mapserver_bindings.cc"
  # to build on linux, comment following 2 lines
  obj.includes = ['/Users/pspencer/Projects/mapserver-5.6/mapserver']
  obj.env["FRAMEWORK"] = "MapServer"
  # to build on linux uncomment following line
  # obj.lib = ["mapserver"]

def shutdown():
  if Options.commands['clean']:
    if exists('mapserver_binding.node'): unlink('mapserver_binding.node')
  else:
    if exists('build/default/mapserver_binding.node') and not exists('mapserver_binding.node'):
      symlink('build/default/mapserver_binding.node', 'mapserver_binding.node')
