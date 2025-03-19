Changelog
=========

1.1.5 (2025-03-19)
------------------

### New Features and Enhancements

* List type was renamed in 24.05.X, and other minor patches ([PR #86][pr86]; thanks @sandynomad)
* Compatibility changes for Slurm 24.11 ([PR #92][pr92]; thanks @sandynomad)
* Test on Rocky 8 and bookworm, drop CentOS 7, bullseye, and unsupported Slurm versions ([PR #87][pr87])

[pr92]: https://github.com/natefoo/slurm-drmaa/pull/92
[pr86]: https://github.com/natefoo/slurm-drmaa/pull/86
[pr87]: https://github.com/natefoo/slurm-drmaa/pull/87

1.1.4 (2023-09-12)
------------------

### New Features and Enhancements

- Slurm 23.02 support ([PR #79][pr79], thanks @sandynomad)
- Test against Slurm 22.05 and 23.02 ([PR #82][pr82])

[pr79]: https://github.com/natefoo/slurm-drmaa/pull/79
[pr82]: https://github.com/natefoo/slurm-drmaa/pull/82

1.1.3 (2021-10-07)
------------------

### New Features and Enhancements

- Retrieve job status from accounting (slurmdb) if already complete (instead of only looking at slurmd) when `SLURM_DRMAA_USE_SLURMDBD` is set. ([PR #39][pr39], [PR #60][pr60]; thanks @holtgrewe)
- Use SlurmDBD when running tests, replace Debian Buster with Bullseye, test against Slurm 21.08 ([PR #59][pr59], [PR #61][pr61])
- Slurm 21.08 support: Prototype of `slurm_kill_job2()` was changed in Slurm 21.08 ([PR #58][pr58]; thanks @sandynomad)
- Add `drmaa-job-ps` command line utility for testing ([drmaa-utils PR #3][utils-pr3])
- RPM release field now contains the version of Slurm installed at build time, when building with the included RPM spec file ([PR #52][pr52]; thanks @kcgthb)

### Bug Fixes

- Array jobs: Don't segfault if parent job doesn't exist ([PR #51][pr51]; thanks @benmwebb)
- Fixes for configuration test order and programs, fix runtime linker path on EL ([Issue #44][issue44], [PR #62][pr62]; thanks @reid-wagner)
- Don't segfault when running `drmaa-*` command line utilities without arguments ([Issue #64][issue64], [drmaa-utils PR #5][utils-pr5]; thanks @richc-at-graphcore)
- Fix path to missing-dev-prog.sh for ragel ([drmaa-utils Issue #2][utils-issue2], [drmaa-utils PR #6][utils-pr6]; thanks @sandynomad)

[pr39]: https://github.com/natefoo/slurm-drmaa/pull/39
[pr51]: https://github.com/natefoo/slurm-drmaa/pull/51
[pr52]: https://github.com/natefoo/slurm-drmaa/pull/52
[pr58]: https://github.com/natefoo/slurm-drmaa/pull/58
[pr59]: https://github.com/natefoo/slurm-drmaa/pull/59
[pr60]: https://github.com/natefoo/slurm-drmaa/pull/60
[pr61]: https://github.com/natefoo/slurm-drmaa/pull/61
[pr62]: https://github.com/natefoo/slurm-drmaa/pull/62
[issue44]: https://github.com/natefoo/slurm-drmaa/issues/44
[issue64]: https://github.com/natefoo/slurm-drmaa/issues/64
[utils-pr3]: https://github.com/natefoo/drmaa-utils/pull/3
[utils-pr5]: https://github.com/natefoo/drmaa-utils/pull/5
[utils-pr6]: https://github.com/natefoo/drmaa-utils/pull/6
[utils-issue2]: https://github.com/natefoo/drmaa-utils/issues/2

1.1.2 (2021-01-27)
------------------

### New Features and Enhancements

- Slurm 20.11 compatibility ([PR #47][pr47]; thanks @sandynomad, @holtgrewe, @aroudgar, @pawsey-kbuckley)
- Tests moved to GitHub Actions ([PR #48][pr48])

[pr47]: https://github.com/natefoo/slurm-drmaa/pull/47
[pr48]: https://github.com/natefoo/slurm-drmaa/pull/48

1.1.1 (2020-03-31)
------------------

### New Features and Enhancements

- Slurm 20.02 compatibility ([PR #34][pr34]; thanks @EricR86)
- Builds are now (minimally) tested against multiple Slurm versions ([PR #36][pr36])

### Bug Fixes

- Minor CI fixes ([PR #35][pr35]; thanks @EricR86)

[pr34]: https://github.com/natefoo/slurm-drmaa/pull/34
[pr35]: https://github.com/natefoo/slurm-drmaa/pull/35
[pr36]: https://github.com/natefoo/slurm-drmaa/pull/36

1.1.0 (2019-03-09)
------------------

### New Features and Enhancements

- Slurm 18.08 compatability
- Support for newer Slurm version configs
- Support `--clusters (-M)` on Slurm 15.08 and later
- Include RPM spec file
- Allow setting Slurm stdout/stderr options in native specification and improve documentation ([PR #9][pr9]; thanks @ljyanesm and @nsoranzo)
- Support newer Slurm job states `JOB_BOOT_FAIL` (14.03), `JOB_DEADLINE` (16.05), and `JOB_OOM` (17.02) ([Issue #12][issue12])
- Add support for `--dependency (-d)` ([b6736da][b6736da], [44240b5][44240b5]; thanks @duffrohde)
- Add support for `--cpus-per-task (-c)` ([8acc159][8acc159]; thanks @atombaby)
- Add support for array jobs in Slurm 14.10 and later ([7b5991e][7b5991e], [a2319d7][a2319d7]; thanks @pkopta at PSNC)
- Update Autotools and usage
- Lots of cleanup for C99 and compiler warnings
- Add basic functional tests and automate tests on PRs in Travis ([PR #22][pr22])

### Bug Fixes

- Don't segfault when a long option that requires a value is passed without one ([Issue #14][issue14])
- Ensure `$SLURM_PRIO_PROCESS`, `$SLURM_SUBMIT_HOST`, `$SLURM_SUBMIT_DIR`, and `$SLURM_UMASK` are set ([Issue #18][issue18])
- Fix the minnodes/maxnodes delimiter used with the `-N`/`--nodes` option ([Issue #2][issue2], [Issue #4][issue4], [PR #11][pr11])
- Fix null pointer dereference segfault when `drmaa_release_*()` is called on an already-freed list ([Issue #6][issue6], [Issue #10][issue10])
- Fix missing break causing a race condition in setting exit status for job failures ([Old Issue #3][old-issue3]; thanks @tbooth)
- Return `DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE` instead of `DRMAA_ERRNO_INTERNAL_ERROR` on status checks when Slurm communication fails ([Issue #1][issue1]; thanks @tbooth)
- job array of size 1 fix ([d399c32][d399c32]; thanks E V)
- control release fix ([8a70cd8][8a70cd8]; thanks E V)

[pr22]: https://github.com/natefoo/slurm-drmaa/pull/22
[pr11]: https://github.com/natefoo/slurm-drmaa/pull/11
[pr9]: https://github.com/natefoo/slurm-drmaa/pull/9
[issue18]: https://github.com/natefoo/slurm-drmaa/issues/18
[issue14]: https://github.com/natefoo/slurm-drmaa/issues/14
[issue12]: https://github.com/natefoo/slurm-drmaa/issues/12
[issue10]: https://github.com/natefoo/slurm-drmaa/issues/10
[issue6]: https://github.com/natefoo/slurm-drmaa/issues/6
[issue4]: https://github.com/natefoo/slurm-drmaa/issues/4
[issue2]: https://github.com/natefoo/slurm-drmaa/issues/2
[issue1]: https://github.com/natefoo/slurm-drmaa/issues/1
[44240b5]: https://github.com/natefoo/slurm-drmaa/commit/44240b514d89f2896ca946763ee7e0fb111098b8
[b6736da]: https://github.com/natefoo/slurm-drmaa/commit/b6736da1382ee63fe5b7f92cfeb032c19278fa78
[8acc159]: https://github.com/natefoo/slurm-drmaa/commit/8acc159de4c5a73c5ebcac78078fb91e6c510a03
[7b5991e]: https://github.com/natefoo/slurm-drmaa/commit/7b5991efc03ab14fdc9e7af67dc91f6085e4d648
[a2319d7]: https://github.com/natefoo/slurm-drmaa/commit/a2319d7ddee4946a6fc466db4abef7588bd424d3
[d399c32]: https://github.com/natefoo/slurm-drmaa/commit/d399c32bbfbe915df97f8e45eb307dd556587631
[8a70cd8]: https://github.com/natefoo/slurm-drmaa/commit/8a70cd8bd8d56cc1e2f5f45768e898a9d4ebe86c
[old-issue3]: https://github.com/natefoo/slurm-drmaa-old/issues/3

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.1.0/slurm-drmaa-1.1.0.tar.gz "slurm-drmaa-1.1.0.tar.gz (941.6 KB)")

1.0.7 (2013-12-02)
------------------

- user supplied (via DRMAA attribute) native specification now takes precedence over the native specification provided in configuration file.

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.7/slurm-drmaa-1.0.7.tar.gz "slurm-drmaa-1.0.7.tar.gz (718.0 KB)")

1.0.6 (2012-11-13)
------------------

- added support for `--gres`, `--no-kill`, `--licenses`, `--mail-type=`, `--no-requeue`, `--exclude`, `--tmp` in native specification attribute
- Implemented handling of missing jobs

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.6/slurm-drmaa-1.0.6.tar.gz "slurm-drmaa-1.0.6.tar.gz (731.5 KB)")

1.0.5 (2012-10-29)
------------------

- better handling of `--time (-t)` (thanks to Roman Valls Guimera)
- added support for `--ntasks (-n)` in native specification attribute
- Fixed `DRMAA_V_EMAIL` attribute handling

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.5/slurm-drmaa-1.0.5.tar.gz "slurm-drmaa-1.0.5.tar.gz (731.6 KB)")

1.0.4 (2012-04-06)
------------------

- support for SLURM 2.3

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.4/slurm-drmaa-1.0.4.tar.gz "slurm-drmaa-1.0.4.tar.gz (729.0 KB)")

1.0.3 (2012-02-01)
------------------

- the `--time` native option support

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.3/slurm-drmaa-1.0.3.tar.gz "slurm-drmaa-1.0.3.tar.gz (729.0 KB)")

1.0.2 (2011-07-19)
------------------

- environment variables are now propagated from submission host to the worker nodes

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.2/slurm_drmaa-1.0.2.tar.gz "slurm_drmaa-1.0.2.tar.gz (727.5 KB)")

1.0.1 (2011-01-02)
------------------

- added support for SLURM 2.2

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.1/slurm_drmaa-1.0.1.tar.gz "slurm_drmaa-1.0.1.tar.gz (709.5 KB)")

1.0.0 (2010-12-17)
------------------

- first public release

[source tarball](https://github.com/natefoo/slurm-drmaa/releases/download/1.0.0/slurm_drmaa-1.0.0.tar.gz "slurm_drmaa-1.0.0.tar.gz (708.9 KB)")
