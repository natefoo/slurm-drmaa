/* $Id$ */
/*
 * PSNC DRMAA for SLURM
 * Copyright (C) 2011 Poznan Supercomputing and Networking Center
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SLURM_DRMAA__SESSION_H
#define __SLURM_DRMAA__SESSION_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/session.h>

typedef struct slurmdrmaa_session_s slurmdrmaa_session_t;

fsd_drmaa_session_t *slurmdrmaa_session_new( const char *contact );

struct slurmdrmaa_session_s {
	fsd_drmaa_session_t super;
};

#endif /* __SLURM_DRMAA__SESSION_H */

