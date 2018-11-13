#!/bin/sh

run() {
	echo "running $@ ($PWD)"
	eval "$@"
}

check()
{
	printf "checking for $1... "
	if ($1 --version < /dev/null > /dev/null 2>&1); then
		echo "yes"
	else
		echo "no"
		exit 1
	fi
}

ACLOCAL=${ACLOCAL:=aclocal}
AUTOHEADER=${AUTOHEADER:=autoheader}
AUTOCONF=${AUTOCONF:=autoconf}
LIBTOOLIZE=${LIBTOOLIZE:=libtoolize}
AUTOMAKE=${AUTOMAKE:=automake}
AUTORECONF=${AUTORECONF:=autoreconf}

check $ACLOCAL
check $AUTOHEADER
check $AUTOCONF
check $LIBTOOLIZE
check $AUTOMAKE
check $AUTORECONF

run autoreconf --install --force -Wall

(cd drmaa_utils && run sh autogen.sh "$@")
