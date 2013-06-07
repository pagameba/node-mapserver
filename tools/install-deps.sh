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
MAPSERVER_COMMIT=$2              # the commit number to checkout (optional)

if [ -z "${PREFIX}" ]; then
    die "usage: install-deps.sh PREFIX [ MAPSERVER_COMMIT ]"
fi

# clone the mapserver repository
git clone https://github.com/mapserver/mapserver.git $PREFIX/mapserver || die "Git clone failed"
cd ${PREFIX}/mapserver || die "Cannot cd to ${PREFIX}/mapserver"
if [ -n "${MAPSERVER_COMMIT}" ]; then
    git checkout $MAPSERVER_COMMIT || die "Cannot checkout ${MAPSERVER_COMMIT}"
fi

# build and install mapserver
autoconf || die "autoconf failed"
./configure --prefix=${PREFIX}/mapserver-install --with-threads || die "configure failed"
make || die "make failed"
make install || die "make install failed"

# point `npm` at the build
npm config set mapserv:lib_dir ${PREFIX}/mapserver-install/lib
npm config set mapserv:include_dir ${PREFIX}/mapserver