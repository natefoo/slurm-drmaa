#!/bin/bash
# build the distribution tarball (in docker)

dimg="${1:-natefoo/slurm-drmaa-dist}"
dctx="$(mktemp -d)"


cat >$dctx/Dockerfile <<'EOF'
FROM debian:stable

LABEL maintainer="Nate Coraor <nate@bx.psu.edu>" \
      description="Image used to build slurm-drmaa release tarballs"

VOLUME ["/slurm-drmaa"]

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -qqy update

# slurm headers/libs are required to generate the build artifacts
RUN apt-get -y install --no-install-recommends build-essential gperf ragel m4 automake autoconf libtool libslurm-dev libslurmdb-dev slurm-wlm git ca-certificates bison
RUN apt-get -y clean

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
./autoclean.sh || true
./autogen.sh
make dist
DISTCHECK_CONFIGURE_FLAGS="CFLAGS='-I../../../../drmaa_utils -I../../..'" make distcheck
EOF

docker build -t $dimg $dctx
docker run -v $(pwd):/slurm-drmaa $dimg
[ -e $dctx ] && rm -rf $dctx
