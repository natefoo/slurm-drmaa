PSNC DRMAA for SLURM
====================

**Please note:** this is only [one slurm-drmaa user](https://github.com/natefoo/)'s import/clone and fork of the canonical [PSNC slurm-drmaa](http://apps.man.poznan.pl/trac/slurm-drmaa).

Introduction
------------

PSNC DRMAA for [Slurm Workload Manager (Slurm)](https://slurm.schedmd.com/) is an implementation of [Open Grid Forum](http://www.gridforum.org/) [DRMAA 1.0](http://www.drmaa.org/) (Distributed Resource Management Application API) [specification](http://www.ogf.org/documents/GFD.133.pdf) for submission and control of jobs to Slurm. Using DRMAA, grid applications builders, portal developers and ISVs can use the same high-level API to link their software with different cluster/resource management systems.

This software also enables the integration of [QCG-Computing](http://www.qoscosgrid.org/trac/qcg-computing/) with the underlying Slurm system for remote multi-user job submission and control over Web Services.

Although this fork is not the canonical source of slurm-drmaa, it has become mildly popular among the [Galaxy](http://galaxyproject.org/) community, especially as development on PSNC slurm-drmaa has slowed folliwing the death of the previous mantainer, Mariusz Mamonski. I will do my best to incorporate both upstream and contributed changes, but credit belongs to the original authors (found below). In this repository, I've imported the SVN history and attempted to attribute contributions correctly.

Download
--------

DRMAA for Slurm is distributed as a source package which can be downloaded via the [Releases](https://github.com/natefoo/slurm-drmaa/releases) section.

Installation
------------

To compile and install the library just go to main source directory and type:

```console
$ ./configure [options] && make
$ sudo make install
```

The library was tested with Slurm versions 14.03, 14.11, and 15.08. If you encountered any problems using the library on the different systems, please see the [contact](#contact) section.

Notable `./configure` script options:

> `--with-slurm-inc` SLURM\_INCLUDE\_PATH
>
> > Path to Slurm header files (i.e. directory containing `slurm/slurm.h` ). By default the library tries to guess the `SLURM_INCLUDE_PATH` and `SLURM_LIBRARY_PATH` based on location of the `srun` executable.

> `--with-slurm-lib` SLURM\_LIBRARY\_PATH
>
> > Path to Slurm libraries (i.e. directory containing `libslurm.a` ).

> `--prefix` INSTALLATION\_DIRECTORY
>
> > Root directory where PSNC DRMAA for Slurm shall be installed. When not given library is installed in `/usr/local`.

> `--enable-debug`
>
> > Compiles library with debugging enabled (with debugging symbols not stripped, without optimizations, and with many log messages enabled). Useful when you are to debug DRMAA enabled application or investigate problems with DRMAA library itself.

There are no unusual requirements for basic usage of library: ANSI C compiler and standard make program should suffice. If you have taken sources directly from this repository or wish to run test-suite you would need additional [developer tools](#developer-tools).

Configuration
-------------

During DRMAA session initialization (`drmaa_init`) library tries to read its configuration parameters from locations: `/etc/slurm_drmaa.conf`, `~/.slurm_drmaa.conf` and from file given in `SLURM_DRMAA_CONF` environment variable (if set to non-empty string). If multiple configuration sources are present then all configurations are merged with values from user-defined files taking precedence (in following order: `$SLURM_DRMAA_CONF`, `~/.slurm_drmaa.conf`, `/etc/slurm_drmaa.conf`).

Currently recognized configuration parameters are:

cache\_job\_state  
According to DRMAA specification every `drmaa_job_ps()` call should query DRM system for job state. With this option one may optimize communication with DRM. If set to positive integer `drmaa_job_ps()` returns remembered job state without communicating with DRM for `cache_job_state` seconds since last update. By default library conforms to the specification (no caching will be performed).

> Type: integer, default: 0

job\_categories  
Dictionary of job categories. Its keys are job categories names mapped to [native specification](#native-specification) strings. Attributes set by job category can be overridden by corresponding DRMAA attributes or native specification. Special category name `default` is used when `drmaa_job_category` job attribute was not set.

> Type: dictionary with string values, default: empty dictionary

### Configuration file syntax

Configuration file is in a form of a dictionary. Dictionary is set of zero or more key-value pairs. Key is a string while value could be a string, an integer or another dictionary.

```
  configuration: dictionary | dictionary_body
  dictionary: '{' dictionary_body '}'
  dictionary_body: (string ':' value ',')*
  value: integer | string | dictionary
  string: unquoted-string | single-quoted-string | double-quoted-string
  unquoted-string: [^ \t\n\r:,0-9][^ \t\n\r:,]*
  single-quoted-string: '[^']*'
  double-quoted-string: "[^"]*"
  integer: [0-9]+
```



Native specification
--------------------

DRMAA interface allows to pass DRM dependent job submission options. Those options may be specified directly by setting `drmaa_native_specification` job template attribute or indirectly by the `drmaa_job_category` job template attribute. The legal format of the native options looks like:

```
  -A My_job_name -s -N 1-10
```

List of parameters that can be passed in the `drmaa_native_specification` attribute:

| Native specification              | Description                                                                                                                      |
|-----------------------------------|----------------------------------------------------------------------------------------------------------------------------------|
| -A, --account=*name*              | Charge job to specified accounts                                                                                                 |
| --acctg-freq=*list*               | Define the job accounting sampling interval                                                                                      |
| --comment=*string*                | An arbitrary comment                                                                                                             |
| -C, --constraint=*list*           | Specify a list of constraints                                                                                                    |
| --contiguous                      | If set, then the allocated nodes must form a contiguous set                                                                      |
| --exclusive                       | Allocate nodenumber of tasks to invoke on each nodes in exclusive mode when cpu consumable resource is enabled                   |
| --gres=*list*                     | Specifies a comma delimited list of generic consumable resources                                                                 |
| -k, --no-kill                     | Do not automatically terminate a job of one of the nodes it has been allocated fails                                             |
| -L, --licenses=*license*          | Specification of licenses                                                                                                        |
| -M, --clusters=*list*             | Comma delimited list of clusters to issue commands to                                                                            |
| --mail-type=*type*                | Notify user by email when certain event types occur. Valid type values are BEGIN, END, FAIL, REQUEUE, and ALL (any state change) |
| --mem=*MB*                        | Minimum amount of real memory                                                                                                    |
| --mem-per-cpu=*MB*                | Maximum amount of real memory per allocated cpu required by a job                                                                |
| --mincpus=*n*                     | Minimum number of logical processors (threads) per node                                                                          |
| -N, --nodes=*minnodes[-maxnodes]* | Number of nodes on which to run                                                                                                  |
| -n, --ntasks=*n*                  | Number of tasks                                                                                                                  |
| --no-requeue                      | Specifies that the batch job should not be requeued after node failure                                                           |
| --ntasks-per-node=*n*             | Number of tasks to invoke on each node                                                                                           |
| -p, --partition=*partition*       | Partition requested                                                                                                              |
| --qos=*qos*                       | Quality of Serice                                                                                                                |
| --requeue                         | If set, permit the job to be requeued                                                                                            |
| --reservation=*name*              | Allocate resources from named reservation                                                                                        |
| -s, --share                       | Job allocation can share nodes with other running jobs                                                                           |
| -t, --time=*hours:minutes*        | Set a maximum job wallclock time                                                                                                 |
| --tmp=*size[units]*               | Specify a minimum amount of temporary disk space                                                                                 |
| -w, --nodelist=*hosts*            | Request a specific list of hosts                                                                                                 |
| -x, --exclude=*nodelist*          | Explicitly exclude certain nodes from the resources granted to the job                                                           |

Description of each parameter can be found in `man sbatch`.

Changelog
---------

-   [1.0.7](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.7/slurm-drmaa-1.0.7.tar.gz "slurm-drmaa-1.0.7.tar.gz (718.0 KB)") - user supplied (via DRMAA attribute) native specification now takes precedence over the native specification provided in configuration file.
-   [1.0.6](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.6/slurm-drmaa-1.0.6.tar.gz "slurm-drmaa-1.0.6.tar.gz (731.5 KB)") - added support for `--gres`, `--no-kill`, `--licenses`, `--mail-type=`, `--no-requeue`, `--exclude`, `--tmp` in native specification attribute. Implemented handling of missing jobs.
-   [1.0.5](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.5/slurm-drmaa-1.0.5.tar.gz "slurm-drmaa-1.0.5.tar.gz (731.6 KB)") - better handling of `--time (-t)` (thanks to Roman Valls Guimera) and added support for `--ntasks (-n)` in native specification attribute. Fixed DRMAA\_V\_EMAIL attribute handling
-   [1.0.4](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.4/slurm-drmaa-1.0.4.tar.gz "slurm-drmaa-1.0.4.tar.gz (729.0 KB)") - support for SLURM 2.3
-   [1.0.3](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.3/slurm-drmaa-1.0.3.tar.gz "slurm-drmaa-1.0.3.tar.gz (729.0 KB)") - the `--time` native option support
-   [1.0.2](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.2/slurm_drmaa-1.0.2.tar.gz "slurm_drmaa-1.0.2.tar.gz (727.5 KB)") - environment variables are now propagated from submission host to the worker nodes
-   [1.0.1](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.1/slurm_drmaa-1.0.1.tar.gz "slurm_drmaa-1.0.1.tar.gz (709.5 KB)") - added support for SLURM 2.2
-   [1.0.0](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.0/slurm_drmaa-1.0.0.tar.gz "slurm_drmaa-1.0.0.tar.gz (708.9 KB)") - first public release

### Known bugs and limitations

Library covers all [DRMAA 1.0 specification](http://www.ogf.org/documents/GFD.133.pdf) with exceptions listed below. It was successfully tested with [Slurm 14.03, 14.11, and 15.08](https://slurm.schedmd.com/). Known limitations:

-   `drmaa_control` options `DRMAA_CONTROL_HOLD`, `DRMAA_CONTROL_RELEASE` are only available for users being Slurm administrators (in version prior 2.2)
-   `drmaa_control` options `DRMAA_CONTROL_SUSPEND`, `DRMAA_CONTROL_RESUME` are only available for users being Slurm administrators
-   `drmaa_wct_slimit` not implemented
-   optional attributes `drmaa_deadline_time`, `drmaa_duration_hlimit`, `drmaa_duration_slimit`, `drmaa_transfer_files` not implemented
-   The SPANK client side (i.e. not remote) plugins chain is not invoked in DRMAA run job call. For this reason we advice you to use [TASK BLOCKS](http://code.google.com/p/slurm-spank-plugins/wiki/UseEnvSyntax#TASK_BLOCKS) in the `UseEnv` SPANK plugin.

Development and Pre-releases
----------------------------

Please note the `./autogen.sh` and `./autoclean.sh` scripts which calls the autotools command chain in appropriate order.

**note:** This repository depends on [DRMAA Utils](https://github.com/natefoo/drmaa-utils/), which is configured as a submodule. When cloning this repository, you should clone recursively, e.g.:

```console
$ git clone --recursive https://github.com/natefoo/slurm-drmaa.git
```

**note:** You need some developer tools to compile the source from git, rather than from a tarball release. Also, the development version may not always compile.

### Developer tools

Although not needed for library user the following tools may be required if you intend to develop PSNC DRMAA for Slurm:

-   GNU autotools
    -   autoconf (tested with version 2.67)
    -   automake (tested with version 1.11)
    -   libtool (tested with version 2.2.8)
    -   m4 (tested with version 1.4.14)
-   [Bison](http://www.gnu.org/software/bison/) parser generator,
-   [RAGEL](http://www.complang.org/ragel/) State Machine Compiler,
-   [gperf](http://www.gnu.org/software/gperf/) gperf - a perfect hash function generator.

Authors
-------

The library was developed by:

-   Michal Matloka - first implementation
-   Mariusz Mamonski - maintainer since version 1.0.3
-   Piotr Kopta - maintainer since version 1.0.7

This library relies heavily on the *Fedstage DRMAA utils* code developed by:

-   Lukasz Ciesnik.

### Contact

In case of any problems or questions regarding PSNC DRMAA for Slurm do not hesitate to contact us:

-   QosCosGrid Development Team - qcg(at)plgrid.pl

You can submit [issues](https://github.com/natefoo/slurm-drmaa/issues) and [pull requests](https://github.com/natefoo/slurm-drmaa/pulls) for this fork of DRMAA for Slurm in GitHub.


### Links

- [PSNC slurm-drmaa:](http://apps.man.poznan.pl/trac/slurm-drmaa) http://apps.man.poznan.pl/trac/slurm-drmaa
- [DRMAA:](http://www.drmaa.org/) http://www.drmaa.org/
- [Open Grid Forum:](http://www.gridforum.org/) http://www.gridforum.org/
- [DRMAA 1.0 specification:](http://www.ogf.org/documents/GFD.133.pdf) http://www.ogf.org/documents/GFD.133.pdf
- [Official DRMAA test-suite:](http://drmaa.org/testsuite.php) http://drmaa.org/testsuite.php
- [Smoa Computing:](http://apps.man.poznan.pl/trac/smoa-comp) http://apps.man.poznan.pl/trac/smoa-comp
- [Slurm Workload Manager:](https://slurm.schedmd.com/) https://slurm.schedmd.com/
- [Bison:](http://www.gnu.org/software/bison/) http://www.gnu.org/software/bison/

### License

Copyright (C) 2011 Poznan Supercomputing and Networking Center

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).
