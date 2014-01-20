#!/bin/sh

##
# Install a checkout of mapserver
#
# This is used by Travis-CI for installing a version of mapserver against which
# node-mapserv can be built and tested.

die() {
    if [ -n "${1}" ]; then
        echo $1
    fi
    exit 1;
}

PREFIX=$1                       # the directory to install into
MAPSERVER_FORK=$2              # mandatory git account name for fork
MAPSERVER_COMMIT=$3              # the commit id to checkout (optional)

if [ -z "${PREFIX}" ]; then
    die "usage: install-deps.sh PREFIX MAPSERVER_FORK [ MAPSERVER_COMMIT ]"
fi
if [ -z "${MAPSERVER_FORK}" ]; then
    die "usage: install-deps.sh PREFIX MAPSERVER_FORK [ MAPSERVER_COMMIT ]"
fi

svn co http://svn.osgeo.org/metacrs/proj/branches/4.8/proj/ $PREFIX/proj
cd ${PREFIX}/proj || die "svn checkout of proj failed"
./configure --prefix=/tmp/proj-install || die "proj configure failed"
make || die "proj make failed"
make install || die "proj make install failed"

# clone the mapserver repository
git clone https://github.com/$MAPSERVER_FORK/mapserver.git $PREFIX/mapserver || die "Git clone failed"
cd ${PREFIX}/mapserver || die "Cannot cd to ${PREFIX}/mapserver"
if [ -n "${MAPSERVER_COMMIT}" ]; then
    git checkout $MAPSERVER_COMMIT || die "Cannot checkout ${MAPSERVER_COMMIT}"
fi

# build and install mapserver
mkdir build
cd build
cmake .. -DWITH_CMAKE_PREFIX_PATH=${PREFIX}/mapserver-install -DWITH_THREAD_SAFETY=1 -DWITH_PROJ=${PREFIX}/proj-install
make || die "make failed"
make install || die "make install failed"

# point `npm` at the build
npm config set mapserver:lib_dir ${PREFIX}/mapserver-install/lib
npm config set mapserver:include_dir ${PREFIX}/mapserver