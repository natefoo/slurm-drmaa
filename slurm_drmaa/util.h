/* $Id: util.h 349 2010-09-30 12:20:44Z mmatloka $ */
/*
 * PSNC DRMAA for SLURM
 * Copyright (C) 2010 Poznan Supercomputing and Networking Center
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

#ifndef __LL_DRMAA__UTIL_H
#define __LL_DRMAA__UTIL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <slurm/slurm.h>

/* Parse time to minutes */
unsigned slurmdrmaa_datetime_parse( const char *string );

void slurmdrmaa_init_job_desc(job_desc_msg_t *job_desc);
void slurmdrmaa_free_job_desc(job_desc_msg_t *job_desc);
void slurmdrmaa_parse_native(job_desc_msg_t *job_desc, const char * value);

#endif /* __SLURM_DRMAA__UTIL_H */
