#!/bin/bash

set -e

source /opt/devkitpro/3dsvars.sh 

export PKG_CONFIG_PATH=/opt/devkitpro/portlibs/3ds/lib/pkgconfig/
export BUILDDIR=/tmp/3ds/__build__
export INSTALLDIR=$PORTLIBS_PREFIX

mkdir -p $BUILDDIR
rm -fr $BUILDDIR
mkdir -p $BUILDDIR

meson setup --buildtype=plain \
    --cross-file=crossfile-3ds.txt \
    --prefix=$INSTALLDIR \
    --libdir=lib \
    --buildtype=release \
    --wrap-mode=nodownload \
    -Ddefault_library=static \
    -Dauto_features=disabled \
    -Dintrospection=disabled \
    -Dbase=enabled \
    -Dgood=enabled \
    -Dbad=disabled \
    -Dugly=enabled \
    -Drtsp_server=disabled \
    -Dgst-plugins-base:filesrc=enabled \
    -Dgst-plugins-base:audiotestsrc=enabled \
    -Dgst-plugins-base:audioconvert=enabled \
    -Dgst-plugins-base:audioresample=enabled \
    -Dgst-plugins-base:videotestsrc=enabled \
    -Dgst-plugins-base:videoconvert=enabled \
    -Dgst-plugins-good:videofilter=enabled \
    -Dgst-plugins-bad:codecs=enabled \
    -Dgst-plugins-good:rtp=enabled \
    -Dgst-plugins-ugly:x264=enabled \
    -Dgpl=enabled \
    -Drs=disabled \
    -Dtests=disabled \
    -Dcoretracers=disabled \
    -Dexamples=disabled \
    -Dnls=disabled \
    -Doption-parsing=false \
    -Dgst_debug=enabled \
    -Dregistry=disabled \
    -Dgst_parse=true \
    -Dtracer_hooks=false \
    -Daudiotestsrc=enabled \
    $BUILDDIR

ninja -C $BUILDDIR
meson install -C $BUILDDIR
