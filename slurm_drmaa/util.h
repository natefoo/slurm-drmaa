/*
 * PSNC DRMAA for SLURM
 * Copyright (C) 2011-2015 Poznan Supercomputing and Networking Center
 * Copyright (C) 2014-2019 The Pennsylvania State University
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

#include <slurm_drmaa/slurm_drmaa.h>

#include <slurm/slurmdb.h>

/* Parse time to minutes */
unsigned slurmdrmaa_datetime_parse( const char *string );

void slurmdrmaa_init_job_desc(job_desc_msg_t *job_desc);
void slurmdrmaa_free_job_desc(job_desc_msg_t *job_desc);
void slurmdrmaa_parse_native(job_desc_msg_t *job_desc, const char * value);
char * slurmdrmaa_set_job_id(job_id_spec_t *job_id_spec);
char * slurmdrmaa_unset_job_id(job_id_spec_t *job_id_spec);
void slurmdrmaa_set_cluster(const char * value);
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(23,0,0)
void slurmdrmaa__init( void );
#endif

#endif /* __SLURM_DRMAA__UTIL_H */
