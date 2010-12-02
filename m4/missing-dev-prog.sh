#!/bin/sh
cat >&2 <<MESSAGE
 * ERROR: $1 was not found at configuration time while some
 * sources are build by it.  Either install $1 or download
 * tarball with generated sources included.
MESSAGE
exit 1
