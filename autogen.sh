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

check $ACLOCAL
check $AUTOHEADER
check $AUTOCONF
check $LIBTOOLIZE
check $AUTOMAKE

mkdir -p scripts

run ${ACLOCAL} -I m4 | grep -v ^/usr/share/aclocal || exit 1
run ${LIBTOOLIZE} --automake --copy --force || exit 1
run ${AUTOHEADER} --warnings=all || exit 1
run ${AUTOMAKE} --foreign --add-missing --copy --warnings=all || exit 1
run ${AUTOCONF} --warnings=all -Wno-obsolete || exit 1

if [ -n "$*" ]; then
	args="$*"
elif [ -f config.log ]; then
	args=`grep '\$ *\./configure ' config.log \
			 | sed 's:^ *\$ *\./configure ::;s:--no-create::;s:--no-recursion::' \
			 2>/dev/null`
fi

eval `grep ^PACKAGE_VERSION= configure`
PACKAGE_RELEASE=`echo ${PACKAGE_VERSION#*-} | sed -e 's/[.-]/_/g'`
sed -i -e "s/^\(Version:\s*\).*$/\1${PACKAGE_VERSION%%-*}/" slurm-drmaa.spec
if [ "${PACKAGE_VERSION}" != "${PACKAGE_RELEASE}" ]; then
    # no dash in $PACKAGE_VERSION so this is not a dev/pre release
    sed -i -e "s/^\(Release:\s*\).*/\11.${PACKAGE_RELEASE}%{?dist}/" slurm-drmaa.spec
fi

(cd drmaa_utils && run sh autogen.sh "$@")
run ./configure ${args}
