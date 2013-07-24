[![Build Status](https://travis-ci.org/pagameba/node-mapserver.png?branch=master)](https://travis-ci.org/pagameba/node-mapserver)

# Overview

node-mapserver is an MIT-licensed node.js extension to the [mapserver](http://mapserver.org/ "MapServer") open source web mapping engine.  It allows you to access all the mapserver goodness for rendering map images in a node.js environment, allowing you to build high-performance, low-latency map servers.

## Getting node-mapserver

Get the source from git and build it on your system against an existing mapserver installation.  You will need to have already installed mapserver plus the development headers using your system's package management tools or built mapserver from source.

See the (MapServer)[http://mapserver.org] website for more details.

### Step 1 - Use The Source, Luke

```
git clone git://github.com/pagameba/node-mapserver.git
cd node-mapserver
```

### Step 2 - Make It So, #1

on linux, you will need to have installed mapserver package (and possibly the -devel package too) using your package management system, or have built it yourself.  If you've installed in, find out where it is installed (perhaps /usr/local/) and use those paths.  If you've built it yourself, you can probably figure out how you need to change this to work for you.

```
CXX="g++ -I/<path-to-install-dir>/include -L/<path-to-install-dir>/lib -lmapserver" node-gyp configure build
```

*NOTE* If your GD library is not 'installed' in a findable location you can add it using -I/<path-to-gd>

*NOTE* If you are building against a mapserver build directory, use -I/<path-to-mapserver-source>

on osx with [kyngchaos](http://kyngchaos.com) frameworks you will still need a copy of the mapserver source from a tarball or svn appropriate to the version you have installed because the framework doesn't (as of the last time I checked) include headers:

```
CXX="g++ -I/<path-to-mapserver-source>" -framework MapServer" node-gyp configure build
```

*NOTE* you will probably get build warnings about the framework being ignored in the compile step, this is normal since the framework only gets used in the linking step.

Building on another system?  Find a problem building on your system?  Submit an issue and let me know what the problem is or how you solved it so we can make this more awesome for everyone.

### Step 3 - Test, Test, Test

```
node tests/tests.js
```

## Using node-mapserver

See the examples provided in the `examples` directory, check out the tests and read up on specific methods in the [documentation](https://github.com/pagameba/node-mapserver/wiki/Api-documentation)

## Documentation

Extensive API documentation is available [in the wiki](https://github.com/pagameba/node-mapserver/wiki/Api-documentation)
