DRMAA for Slurm
===============

**Please note:** DRMAA for Slurm is a continuation of [PSNC DRMAA for SLURM][psnc-slurm-drmaa-trac] by [an unrelated developer][natefoo].

[psnc-slurm-drmaa-trac]: http://apps.man.poznan.pl/trac/slurm-drmaa
[natefoo]: https://github.com/natefoo/

Introduction
------------

DRMAA for [Slurm Workload Manager][slurm] (Slurm) is an implementation of [Open Grid Forum][ogf] [Distributed Resource Management Application API][drmaa] (DRMAA) [version 1][drmaa-v133] for submission and control of jobs to Slurm. Using DRMAA, grid applications builders, portal developers and ISVs can use the same high-level API to link their software with different cluster/resource management systems.

[slurm]: https://slurm.schedmd.com/
[ogf]: http://www.gridforum.org/
[drmaa]: http://www.drmaa.org/
[drmaa-v133]: http://www.ogf.org/documents/GFD.133.pdf

### History

DRMAA for Slurm was originally developed at the [Poznań Supercomputing and Networking Center][psnc] as [PSNC DRMAA for SLURM][psnc-slurm-drmaa-trac]. Following the unexpected death in 2013 of its primary maintainer, Mariusz Mamoński, there has been little additional development from PSNC, and no new releases.

[This fork][slurm-drmaa] is maintained by [Nate Coraor][natefoo] and was originally created in 2014 to add support for Slurm's `--clusters` (`-M`) option. Since that time, others have found it useful and additional features and bug fixes have been added. However, the majority of the credit for this work belongs to the original authors (found below). In 2017, current maintainer Piotr Kopta created [psnc-apps/slurm-drmaa][psnc-slurm-drmaa-github] in Github, a snapshot of the unreleased 1.2.0 version (upon which this fork is also based) that has seen occasional work.

The title of the software in this fork has been changed from *PSNC DRMAA for SLURM* to simply *DRMAA for Slurm*. This change was made in order to alleviate confusion and differentiate from the canonical version. Additionally, this fork is not affiliated with PSNC; as such, releasing this software under PSNC's name would not be appropriate. However, this fork's maintainer is incredibly grateful for the work of the original authors of this software and the name change is in no way intended to minimize the efforts of those people.

[psnc]: http://www.man.poznan.pl/
[slurm-drmaa]: https://github.com/natefoo/slurm-drmaa/
[psnc-slurm-drmaa-github]: https://github.com/psnc-apps/slurm-drmaa

Download
--------

DRMAA for Slurm is distributed as a source package which can be downloaded via the [Releases](https://github.com/natefoo/slurm-drmaa/releases) section.

Installation
------------

To compile and install the library, go to main source directory and type:

```console
$ ./configure [options] && make
$ sudo make install
```

The library uses the standard GNU Autotools system, so standard `configure` arguments are available; see `./configure --help` for a full list. The library was tested with Slurm versions 16.05 and 18.08. If you encountered any problems using the library on different systems, please see the [contact](#contact) section.

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

There are no unusual requirements for basic usage of library: a C99 compiler and standard make program should suffice. If you have taken sources directly from this repository or wish to run test-suite you would need additional [developer tools](#developer-tools).

Configuration
-------------

During DRMAA session initialization (`drmaa_init`), the library tries to read its configuration parameters from locations: `/etc/slurm_drmaa.conf`, `~/.slurm_drmaa.conf` and from the file given in the `$SLURM_DRMAA_CONF` environment variable (if set to a non-empty string). If multiple configuration sources are present then all configurations are merged with values from user-defined files taking precedence (in the following order: `$SLURM_DRMAA_CONF`, `~/.slurm_drmaa.conf`, `/etc/slurm_drmaa.conf`).

Currently recognized configuration parameters are:

> `cache_job_state`
>
> > According to the DRMAA specification, every `drmaa_job_ps()` call should query the DRM system for job state. With this option one may optimize communication with the DRM. If set to a positive integer, `drmaa_job_ps()` returns the remembered job state without communicating with the DRM for `cache_job_state` seconds since the last update. By default the library conforms to the specification (no caching will be performed).
> >
> > Type: integer, default: 0

> `job_categories`
>
> > Dictionary of job categories. Its keys are job categories names mapped to [native specification](#native-specification) strings. Attributes set by job category can be overridden by corresponding DRMAA attributes or native specification. The special category name `default` is used when the `drmaa_job_category` job attribute is not set.
> >
> > Type: dictionary with string values, default: empty dictionary

### Configuration file syntax

The configuration file is in a form of a dictionary. A dictionary is set of zero or more key-value pairs. A key is a string, while a value can be a string, an integer or another dictionary.

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

The DRMAA interface allows passing DRM-dependent job submission options. Those options may be specified directly by setting the `drmaa_native_specification` job template attribute or indirectly by the `drmaa_job_category` job template attribute. The legal format of the native options looks like:

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
| -c, --cpus-per-task=*n*           | Number of processors per task                                                                                                    |
| --contiguous                      | If set, then the allocated nodes must form a contiguous set                                                                      |
| -d, --dependency=*list*           | Defer the start of this job until the specified dependencies have been satisfied completed                                       |
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
| --tmp=*size[units]*               | Specify a minimum amount of temporary disk space                                                                                 |
| -w, --nodelist=*hosts*            | Request a specific list of hosts                                                                                                 |
| -x, --exclude=*nodelist*          | Explicitly exclude certain nodes from the resources granted to the job                                                           |

Additionally, the following parameters to `drmaa_native_specification` are supported, but their use is discouraged in
favor of the corresponding DRMAA job attributes:

| Native specification       | DRMAA job attribute | Description                                                                                   |
|----------------------------|---------------------|-----------------------------------------------------------------------------------------------|
| -e, --error=*pattern*      | drmaa_output_path   | Connect the batch script's standard error directly to the file name specified in the pattern  |
| -J, --job-name=*name*      | drmaa_job_name      | Specify a name for the job allocation                                                         |
| -o, --output=*pattern*     | drmaa_error_path    | Connect the batch script's standard output directly to the file name specified in the pattern |
| -t, --time=*hours:minutes* | drmaa_wct_hlimit    | Set a maximum job wallclock time                                                              |

Descriptions of each parameter can be found in `man sbatch`.

Changelog
---------

See [CHANGELOG.md](CHANGELOG.md)

### Known bugs and limitations

The library covers all of the [DRMAA 1.0 specification][drmaa-v133] with exceptions listed below. It was successfully tested with [Slurm 16.05 and 18.08][slurm]. Known limitations:

-   `drmaa_control` options `DRMAA_CONTROL_HOLD`, `DRMAA_CONTROL_RELEASE` are only available for users being Slurm administrators (in version prior 2.2)
-   `drmaa_control` options `DRMAA_CONTROL_SUSPEND`, `DRMAA_CONTROL_RESUME` are only available for users being Slurm administrators
-   `drmaa_wct_slimit` not implemented
-   optional attributes `drmaa_deadline_time`, `drmaa_duration_hlimit`, `drmaa_duration_slimit`, `drmaa_transfer_files` not implemented
-   The SPANK client side (i.e. not remote) plugins chain is not invoked in DRMAA run job call. For this reason we advice you to use [TASK BLOCKS](http://code.google.com/p/slurm-spank-plugins/wiki/UseEnvSyntax#TASK_BLOCKS) in the `UseEnv` SPANK plugin.

Development and Pre-releases
----------------------------

**note:** This repository depends on [FedStage DRMAA Utils][drmaa-utils], which is configured as a submodule. When cloning this repository, you should clone recursively, e.g.:

```console
$ git clone --recursive https://github.com/natefoo/slurm-drmaa.git
```

The source repository does not contain Autotools-generated artifacts such as `configure` and `Makefile`. Please note the `./autogen.sh` and `./autoclean.sh` scripts which call the Autotools command chain in the appropriate order to generate these artifacts.

**note:** You need some developer tools to compile the source from git.

[drmaa-utils]: https://github.com/natefoo/drmaa-utils/

### Developer tools

Although not needed to use the library or to compile from source distribution tarballs, user the following tools may be required if you intend to develop DRMAA for Slurm from git:

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

-   Lukasz Ciesnik

The maintainer of [this fork][slurm-drmaa] is:

-   Nate Coraor

with [additional contributors][https://github.com/natefoo/slurm-drmaa/graphs/contributors].

### Contact

You can submit [issues](https://github.com/natefoo/slurm-drmaa/issues) and [pull requests](https://github.com/natefoo/slurm-drmaa/pulls) for DRMAA for Slurm in GitHub.

Links
-----

- [PSNC DRMAA for SLURM][psnc-slurm-drmaa-trac]
- [PSNC DRMAA for SLURM][psnc-slurm-drmaa-github] in Github
- [DRMAA][drmaa]
- [Open Grid Forum][ogf]
- [DRMAA 1.0 specification][drmaa-v133]
- [Official DRMAA test-suite](http://drmaa.org/testsuite.php)
- [Slurm Workload Manager][slurm]

### Software using DRMAA for Slurm

- [QCG-Computing](http://www.qoscosgrid.org/trac/qcg-computing/): remote multi-user job submission and control over Web Services
- [Galaxy](https://galaxyproject.org/): an open, web-based platform for accessible, reproducible, and transparent computational biomedical research

License
-------

Copyright (C) 2011-2015 Poznan Supercomputing and Networking Center
Copyright (C) 2014-2019 The Pennsylvania State University

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

Some portions of this program are copied or derived from Slurm, which is licensed under the GNU General Public License Version 2. For details, including the list of Slurm copyright holders,  see <[https://slurm.schedmd.com/](https://slurm.schedmd.com/)>.
