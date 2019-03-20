#!/bin/bash
set -e
# build the distribution tarball (in docker)

dimg="${1:-natefoo/slurm-drmaa-dist}"
dctx="$(mktemp -d)"


cat >$dctx/Dockerfile <<'EOF'
FROM debian:stable

LABEL maintainer="Nate Coraor <nate@bx.psu.edu>" \
      description="Image used to build slurm-drmaa release tarballs"

VOLUME ["/slurm-drmaa"]

ENV DEBIAN_FRONTEND noninteractive

# slurm headers/libs are required to generate the build artifacts
RUN apt-get -qqy update && apt-get -y install --no-install-recommends build-essential gperf ragel m4 automake autoconf \
    libtool libslurm-dev libslurmdb-dev slurm-wlm git ca-certificates bison
RUN apt-get -y clean

RUN git clone https://github.com/bats-core/bats-core.git/ /bats
RUN echo 'PATH=/bats/bin:${PATH}' >> /etc/profile

ADD docker-make-release.sh /

ENTRYPOINT ["/bin/bash", "/docker-make-release.sh"]
EOF

cat >$dctx/docker-make-release.sh <<'EOF'
#!/bin/bash
set -e

if [ $(id -u) -eq 0 ]; then
    uid=$(stat -c %u /slurm-drmaa)
    gid=$(stat -c %g /slurm-drmaa)
    if [ "$uid" -ne 0 ]; then
        [ "$gid" -ne 0 ] && groupadd -g $gid build
        useradd -u $uid -g $gid -s /bin/bash -d /slurm-drmaa build
        exec su - build -c "/bin/bash /docker-make-release.sh"
    fi
fi

[ ! -e "/slurm-drmaa/autogen.sh" ] && git clone --recursive https://github.com/natefoo/slurm-drmaa.git /slurm-drmaa

cd /slurm-drmaa

# replace rev fetching command with its output
sed -i -E -e 's/^(AC_INIT\(.*)m4_esyscmd_s\([^)]+\)(.*)$/\1'$(eval $(grep '^AC_INIT(' configure.ac | sed -E 's/^AC_INIT\(.*m4_esyscmd_s\(([^)]+).*$/\1/'))'\2/' \
          -e 's/^(AC_REVISION\()\[m4_esyscmd_s\([^)]+\)\](.*)$/\1['$(eval $(grep '^AC_REVISION(' configure.ac | sed -E 's/^AC_REVISION\(\[m4_esyscmd_s\(\[([^]]+).*$/\1/'))']\2/' configure.ac

# also for drmaa_utils
cd drmaa_utils
sed -i -E -e 's/^(AC_REVISION\()\[m4_esyscmd_s\([^)]+\)\](.*)$/\1['$(eval $(grep '^AC_REVISION(' configure.ac | sed -E 's/^AC_REVISION\(\[m4_esyscmd_s\(\[([^]]+).*$/\1/'))']\2/' configure.ac
cd ..

./autoclean.sh || true
./autogen.sh
./configure

# fix up RPM specfile
eval `grep ^PACKAGE_VERSION= configure`
PACKAGE_RELEASE=`echo ${PACKAGE_VERSION#*-} | sed -e 's/[.-]/_/g'`
sed -i -e "s/^\(Version:\s*\).*$/\1${PACKAGE_VERSION%%-*}/" slurm-drmaa.spec
if [ "${PACKAGE_VERSION}" != "${PACKAGE_RELEASE}" ]; then
    # no dash in $PACKAGE_VERSION so this is not a dev/pre release
    sed -i -e "s/^\(Release:\s*\).*/\11.${PACKAGE_RELEASE}%{?dist}/" slurm-drmaa.spec
fi

make dist
DISTCHECK_CONFIGURE_FLAGS="CFLAGS='-I../../../../drmaa_utils -I../../..'" make distcheck
EOF

docker build -t $dimg $dctx
docker run -v $(pwd):/slurm-drmaa $dimg
[ -e $dctx ] && rm -rf $dctx
