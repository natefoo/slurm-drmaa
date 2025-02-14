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

#include <string.h>
#include <unistd.h>

#include <drmaa_utils/iter.h>
#include <drmaa_utils/conf.h>
#include <slurm_drmaa/job.h>
#include <slurm_drmaa/session.h>
#include <slurm_drmaa/util.h>
#include <slurm_drmaa/slurm_missing.h>

#include <slurm/slurmdb.h>

static char *slurmdrmaa_session_run_job(fsd_drmaa_session_t *self, const fsd_template_t *jt);

static fsd_iter_t *slurmdrmaa_session_run_bulk(	fsd_drmaa_session_t *self,const fsd_template_t *jt, int start, int end, int incr );

static fsd_job_t *slurmdrmaa_session_new_job( fsd_drmaa_session_t *self, const char *job_id );

slurmdb_cluster_rec_t *working_cluster_rec = NULL;

fsd_drmaa_session_t *
slurmdrmaa_session_new( const char *contact )
{
	slurmdrmaa_session_t *volatile self = NULL;
	TRY
	 {
		self = (slurmdrmaa_session_t*)fsd_drmaa_session_new(contact);

		fsd_realloc( self, 1, slurmdrmaa_session_t );

		self->super.run_job = slurmdrmaa_session_run_job;
		self->super.run_bulk = slurmdrmaa_session_run_bulk;
		self->super.new_job = slurmdrmaa_session_new_job;

		self->super.load_configuration( &self->super, "slurm_drmaa" );
	 }
	EXCEPT_DEFAULT
	 {
		fsd_free( self );
		fsd_exc_reraise();
	 }
	END_TRY
	return (fsd_drmaa_session_t*)self;
}


char *
slurmdrmaa_session_run_job(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt
		)
{
	char *volatile job_id = NULL;
	fsd_iter_t *volatile job_ids = NULL;

	TRY
	 {
		job_ids = self->run_bulk( self, jt, 0, 0, 0 ); /* single job run as bulk job specialization */
		job_id = fsd_strdup( job_ids->next( job_ids ) );
	 }
	FINALLY
	 {
		if( job_ids )
			job_ids->destroy( job_ids );
	 }
	END_TRY
	return job_id;
}


fsd_iter_t *
slurmdrmaa_session_run_bulk(
		fsd_drmaa_session_t *self,
		const fsd_template_t *jt,
		int start, int end, int incr )
{
	fsd_job_t *volatile job = NULL;
	char **volatile job_ids = NULL;
	volatile unsigned n_jobs = 1;
	volatile bool connection_lock = false;
	fsd_environ_t *volatile env = NULL;
	job_desc_msg_t job_desc;
	submit_response_msg_t *submit_response = NULL;
	job_info_msg_t *job_info = NULL;
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(14, 10, 0)
	int v = 0;
#endif

	/* zero out the struct, and set default vaules */
	slurm_init_job_desc_msg( &job_desc );

	TRY
	 {
		unsigned i;
		if ( start != end ) {
			n_jobs = (end - start) / incr + 1;
		} else {
			n_jobs = 1;
		}

		fsd_calloc( job_ids, n_jobs+1, char* );

		if ( start != 0 || end != 0 || incr != 0 ) {
			job_desc.array_inx = fsd_asprintf( "%d-%d:%d", start, end, incr );
		}

		connection_lock = fsd_mutex_lock( &self->drm_connection_mutex );
		slurmdrmaa_job_create_req( self, jt, (fsd_environ_t**)&env , &job_desc );
		int _slurm_errno;
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(24,11,0)
		if (_slurm_errno = slurm_submit_batch_job(&job_desc,&submit_response)) {
#else
		if (slurm_submit_batch_job(&job_desc,&submit_response)) {
			_slurm_errno = slurm_get_errno();
#endif
			if (_slurm_errno == EAGAIN ||
			    (_slurm_errno >= 5000 && _slurm_errno < 6000)) {
				fsd_exc_raise_fmt(FSD_ERRNO_DRM_COMMUNICATION_FAILURE,"slurm_submit_batch_job error: %s", slurm_strerror(_slurm_errno));
			} else if (_slurm_errno >= 2000 && _slurm_errno < 4000) {
				fsd_exc_raise_fmt(FSD_ERRNO_DENIED_BY_DRM,"slurm_submit_batch_job error: %s", slurm_strerror(_slurm_errno));
			} else {
				fsd_exc_raise_fmt(FSD_ERRNO_INTERNAL_ERROR,"slurm_submit_batch_job error (%d): %s", _slurm_errno, slurm_strerror(_slurm_errno));
			}
		}

		connection_lock = fsd_mutex_unlock( &self->drm_connection_mutex );

		if (!working_cluster_rec)
			fsd_log_debug(("job %u submitted", submit_response->job_id));
		else
			fsd_log_debug(("job %u submitted on cluster %s", submit_response->job_id, working_cluster_rec->name));

		if ( start != 0 || end != 0 || incr != 0 ) {
			int _serrno;
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(24,11,0)
			if (( _serrno = slurm_load_job( &job_info, submit_response->job_id, 0)) != SLURM_SUCCESS) {
#else
			if ( SLURM_SUCCESS != slurm_load_job( &job_info, submit_response->job_id, 0) ) {
				_serrno = slurm_get_errno();
#endif
				fsd_exc_raise_fmt( FSD_ERRNO_INTERNAL_ERROR,"slurm_load_job: %s",slurm_strerror(_serrno));
			}
			else {
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(14, 10, 0)
				for (i = 0, v = start; i < n_jobs; i++, v += incr) {
					job_ids[i] = fsd_asprintf("%d_%d", submit_response->job_id, v);
#else
				fsd_assert(  job_info->record_count == n_jobs );
				for (i=0; i < job_info->record_count; i++) {
					job_ids[i] = fsd_asprintf("%d", job_info->job_array[i].job_id);
#endif
					if (working_cluster_rec)
						job_ids[i] = fsd_asprintf("%s.%s", job_ids[i], working_cluster_rec->name);
					job = slurmdrmaa_job_new( fsd_strdup(job_ids[i]) );
					job->session = self;
					job->submit_time = time(NULL);
					self->jobs->add( self->jobs, job );
					job->release( job );
					job = NULL;
				}
			}
		} else {
			if (!working_cluster_rec)
				job_ids[0] = fsd_asprintf( "%d", submit_response->job_id); /* .0*/
			else
				job_ids[0] = fsd_asprintf("%d.%s",submit_response->job_id,working_cluster_rec->name);

			job = slurmdrmaa_job_new( fsd_strdup(job_ids[0]) ); /* TODO: ??? */
			job->session = self;
			job->submit_time = time(NULL);
			self->jobs->add( self->jobs, job );
			job->release( job );
			job = NULL;
		}
	 }
	ELSE
	 {
		if ( !connection_lock )
			connection_lock = fsd_mutex_lock( &self->drm_connection_mutex );
	 }
	FINALLY
	 {


		if( connection_lock )
			fsd_mutex_unlock( &self->drm_connection_mutex );

		if( submit_response )
			slurm_free_submit_response_response_msg ( submit_response );

		if( job )
			job->release( job );

		if (working_cluster_rec)
			slurmdb_destroy_cluster_rec(working_cluster_rec);
		working_cluster_rec = NULL;

		if( fsd_exc_get() != NULL )
			fsd_free_vector( job_ids );

		slurmdrmaa_free_job_desc(&job_desc);
	 }
	END_TRY

	return fsd_iter_new( job_ids, n_jobs );
}


fsd_job_t *
slurmdrmaa_session_new_job( fsd_drmaa_session_t *self, const char *job_id )
{
	fsd_job_t *job;
	job = slurmdrmaa_job_new( fsd_strdup(job_id) );
	job->session = self;
	return job;
}
