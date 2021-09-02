ARG from
FROM $from
ARG install_cmd
ARG uid
ARG gid
ARG version
RUN command -v apt-get || { curl -o /etc/yum.repos.d/slurm.repo https://depot.galaxyproject.org/yum/package/slurm/$version/slurm.repo && yum install -y '@development tools' mariadb-server slurm slurm-devel slurm-slurmctld slurm-slurmd slurm-slurmdbd && useradd -m -d /home/slurm slurm; }
RUN command -v yum || { apt-get -qq update && apt-get install --no-install-recommends -y build-essential mariadb-server slurm-wlm slurmdbd libslurm-dev libslurm-dev; }
RUN mkdir -p /etc/munge /var/run/munge && [ -f /etc/munge/munge.key ] || dd if=/dev/urandom bs=1 count=1024 >/etc/munge/munge.key
RUN mkdir -m 1777 /slurm && groupadd -g $gid drmaa && useradd -u $uid -g $gid -m -d /drmaa drmaa
VOLUME ["/ci"]
ENV SLURM_CONF /etc/slurm/slurm.conf
# slurmdbd gets picky about ownership and permissions
#ENV SLURMDBD_CONF /ci/slurmdbd.conf
ENV PATH /ci/bats/bin:${PATH}
COPY runslurm.sh /runslurm.sh
