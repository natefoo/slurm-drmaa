#!/bin/sh

echo "starting slurm daemons..."

set -x
munged --force
slurmctld -L /ci/slurmctld.log
slurmd -L /ci/slurmd.log
{ set +x; }  2>/dev/null

while [ ! -f /ci/slurmctld.log ]; do echo "waiting for slurmctld.log..."; sleep 1; done
while [ ! -f /ci/slurmd.log ]; do echo "waiting for slurmd.log..."; sleep 1; done

chmod 0644 /ci/slurmctld.log /ci/slurmd.log

while true; do
    sleep 5
done
