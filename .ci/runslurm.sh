#!/bin/sh

echo "configuring database for slurmdbd..."

eval `grep ^ID= /etc/os-release`

case $ID in
    debian)
        install -m 755 -o mysql -g root -d /var/run/mysqld
        usermod -d /var/lib/mysql mysql
        su - mysql -s /bin/sh -c /usr/sbin/mysqld &
        sock=/var/run/mysqld/mysqld.sock
        ;;
    centos)
        /usr/libexec/mariadb-prepare-db-dir mariadb.service
        /usr/bin/mysqld_safe --basedir=/usr &
        #/usr/libexec/mariadb-wait-ready $!
        sock=/var/lib/mysql/mysql.sock
        ;;
esac

while [ ! -S "$sock" ]; do echo "waiting for mysqld.sock..."; sleep 1; done

mysql -e "CREATE USER slurm"
mysql -e "GRANT ALL PRIVILEGES ON *.* TO 'slurm'@'%'"
# centos seems to need this
mysql -e "GRANT ALL PRIVILEGES ON *.* TO ''@'localhost'"

echo "starting slurm daemons..."

set -x
munged --force
#slurmdbd -L /ci/slurmdbd.log
slurmdbd
{ set +x; }  2>/dev/null
while [ -z "`sacctmgr -no list stats`" ]; do echo "waiting for slurmdbd responsiveness..."; sleep 1; done
set -x
sacctmgr -i create cluster test
slurmctld -L /ci/slurmctld.log
slurmd -L /ci/slurmd.log
{ set +x; }  2>/dev/null

while [ ! -f /ci/slurmdbd.log ]; do echo "waiting for slurmdbd.log..."; sleep 1; done
while [ ! -f /ci/slurmctld.log ]; do echo "waiting for slurmctld.log..."; sleep 1; done
while [ ! -f /ci/slurmd.log ]; do echo "waiting for slurmd.log..."; sleep 1; done

chmod 0644 /ci/slurmdbd.log /ci/slurmctld.log /ci/slurmd.log

echo "All services started, entering loop"

while true; do
    sleep 5
done
