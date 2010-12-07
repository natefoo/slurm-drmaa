#!/bin/sh
# $Id$

echo "Removing all generated files. ($PWD)"

make maintainer-clean

rm -f aclocal.m4 configure configure.scan config.h.in confdefs.h libtool

rm -rf autom4te.cache
rm -rf scripts

find . -name Makefile.in -exec rm -f {} \;
find . -name Makefile -exec rm -f {} \;
find . -name \*~ -exec rm -f {} \;
find . -name \.#* -exec rm -f {} \;
find . -name \*.core -exec rm -f {} \;
find . -name \*.log -exec rm -f {} \;
find . -name \*.a -exec rm -f {} \;
find . -name \*.o -exec rm -f {} \;
find . -name \*.lo* -exec rm -f {} \;
find . -name \*.la* -exec rm -f {} \;

(cd drmaa_utils && sh autoclean.sh "$@")
echo "done."
