#
# DRMAA for Slurm
# Copyright (C) 2014-2019 The Pennsylvania State University
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TESTS = functional-basic.bats
TEST_EXTENSIONS = .bats
BATS_LOG_COMPILER = bats

check_PROGRAMS = slurm_ping

slurm_ping_CPPFLAGS = @SLURM_INCLUDES@
slurm_ping_LDFLAGS = @SLURM_LDFLAGS@
slurm_ping_LDADD = @SLURM_LIBS@

EXTRA_DIST = $(TESTS) helper_slurm.bash

clean-local:
	rm -rf _slurm
	rm -f _slurm_available
