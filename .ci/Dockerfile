ARG from
FROM $from
ARG install_cmd
ARG uid
ARG gid
RUN command -v apt-get || { curl -o /etc/yum.repos.d/galaxy.repo https://depot.galaxyproject.org/yum/galaxy.repo && yum install -y '@development tools' slurm slurm-devel slurm-slurmctld slurm-slurmd && useradd -m -d /home/slurm slurm; }
RUN command -v yum || { apt-get -qq update && apt-get install -y build-essential slurm-wlm libslurm-dev libslurmdb-dev; }
RUN mkdir -p /etc/munge /var/run/munge && [ -f /etc/munge/munge.key ] || dd if=/dev/urandom bs=1 count=1024 >/etc/munge/munge.key
RUN mkdir -m 1777 /slurm && groupadd -g $gid drmaa && useradd -u $uid -g $gid -m -d /drmaa drmaa
VOLUME ["/ci"]
ENV SLURM_CONF /ci/slurm.conf
ENV PATH /ci/bats/bin:${PATH}
COPY runslurm.sh /runslurm.sh
