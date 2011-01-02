# $Id$
#
# PSNC DRMAA for LoadLeveler
# Copyright (C) 2011 Poznan Supercomputing and Networking Center
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



lib_LTLIBRARIES = libdrmaa.la
libdrmaa_la_SOURCES = \
 drmaa.c \
 job.c job.h \
 session.c session.h \
 util.c util.h
libdrmaa_la_CPPFLAGS = @SLURM_INCLUDES@ -I$(top_srcdir)/drmaa_utils/ 
if GCC
libdrmaa_la_CPPFLAGS += -Wno-long-long 
endif

libdrmaa_la_LIBADD = ../drmaa_utils/drmaa_utils/libdrmaa_utils.la @SLURM_LIBS@
libdrmaa_la_LDFLAGS = @SLURM_LDFLAGS@ -version-info @SLURM_DRMAA_VERSION_INFO@

dist_sysconf_DATA = slurm_drmaa.conf.example
