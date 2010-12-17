/* $Id$ */
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

#ifndef __SLURM_DRMAA__JOB_H
#define __SLURM_DRMAA__JOB_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <drmaa_utils/job.h>
#include <drmaa_utils/environ.h>

#include <slurm/slurm.h>

typedef struct slurmdrmaa_job_s slurmdrmaa_job_t;

fsd_job_t *
slurmdrmaa_job_new(char *job_id );

struct slurmdrmaa_job_s {
	fsd_job_t super;
	
	/* job priority before hold */
	uint32_t old_priority;
	bool user_suspended;
};

void slurmdrmaa_job_create_req(fsd_drmaa_session_t *session,const fsd_template_t *jt,fsd_environ_t **envp, job_desc_msg_t * job_desc,	int n_job );
void slurmdrmaa_job_create(fsd_drmaa_session_t *session,const fsd_template_t *jt,fsd_environ_t **envp,fsd_expand_drmaa_ph_t *expand, job_desc_msg_t * job_desc,	int n_job);

#endif /* __SLURM_DRMAA__JOB_H */

