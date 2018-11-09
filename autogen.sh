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

eval `grep ^PACKAGE_VERSION= configure`
PACKAGE_RELEASE=`echo ${PACKAGE_VERSION#*-} | sed -e 's/[.-]/_/g'`
sed -i -e "s/^\(Version:\s*\).*$/\1${PACKAGE_VERSION%%-*}/" slurm-drmaa.spec
if [ "${PACKAGE_VERSION}" != "${PACKAGE_RELEASE}" ]; then
    # no dash in $PACKAGE_VERSION so this is not a dev/pre release
    sed -i -e "s/^\(Release:\s*\).*/\11.${PACKAGE_RELEASE}%{?dist}/" slurm-drmaa.spec
fi

(cd drmaa_utils && run sh autogen.sh "$@")
