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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/conf.h>
#include <drmaa_utils/datetime.h>
#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/drmaa_util.h>
#include <drmaa_utils/environ.h>
#include <drmaa_utils/template.h>

#include <slurm_drmaa/job.h>
#include <slurm_drmaa/session.h>
#include <slurm_drmaa/util.h>

#include <slurm/slurm.h>
#include <stdint.h>

static void
slurmdrmaa_job_control( fsd_job_t *self, int action )
{
	slurmdrmaa_job_t *slurm_self = (slurmdrmaa_job_t*)self;
	job_desc_msg_t job_desc;

	fsd_log_enter(( "({job_id=%s}, action=%d)", self->job_id, action ));

	fsd_mutex_lock( &self->session->drm_connection_mutex );
	TRY
	 {
		switch( action )
		 {
			case DRMAA_CONTROL_SUSPEND:
				if(slurm_suspend(fsd_atoi(self->job_id)) == -1) {
					fsd_exc_raise_fmt(	FSD_ERRNO_INTERNAL_ERROR,"slurm_suspend error: %s,job_id: %s",slurm_strerror(slurm_get_errno()),self->job_id);
				}
				slurm_self->user_suspended = true;
				break;
			case DRMAA_CONTROL_HOLD:
				/* change priority to 0*/
				slurm_init_job_desc_msg(&job_desc);
				slurm_self->old_priority = job_desc.priority;
				job_desc.job_id = atoi(self->job_id);
				job_desc.priority = 0;
				job_desc.alloc_sid = 0;
				if(slurm_update_job(&job_desc) == -1) {
					fsd_exc_raise_fmt(	FSD_ERRNO_INTERNAL_ERROR,"slurm_update_job error: %s,job_id: %s",slurm_strerror(slurm_get_errno()),self->job_id);
				}
				break;
			case DRMAA_CONTROL_RESUME:
				if(slurm_resume(fsd_atoi(self->job_id)) == -1) {
					fsd_exc_raise_fmt(	FSD_ERRNO_INTERNAL_ERROR,"slurm_resume error: %s,job_id: %s",slurm_strerror(slurm_get_errno()),self->job_id);
				}
				slurm_self->user_suspended = false;
				break;
			case DRMAA_CONTROL_RELEASE:
			  /* change priority back*/
			  	slurm_init_job_desc_msg(&job_desc);
				job_desc.priority = 1;
				job_desc.job_id = atoi(self->job_id);
				if(slurm_update_job(&job_desc) == -1) {
					fsd_exc_raise_fmt(	FSD_ERRNO_INTERNAL_ERROR,"slurm_update_job error: %s,job_id: %s",slurm_strerror(slurm_get_errno()),self->job_id);
				}
				break;
			case DRMAA_CONTROL_TERMINATE:
				if(slurm_kill_job(fsd_atoi(self->job_id),SIGKILL,0) == -1) {
					fsd_exc_raise_fmt(	FSD_ERRNO_INTERNAL_ERROR,"slurm_terminate_job error: %s,job_id: %s",slurm_strerror(slurm_get_errno()),self->job_id);
				}
				break;
			default:
				fsd_exc_raise_fmt(
						FSD_ERRNO_INVALID_ARGUMENT,
						"job::control: unknown action %d", action );
		 }
					
		fsd_log_debug(("job::control: successful"));
	 }
	FINALLY
	 {
		fsd_mutex_unlock( &self->session->drm_connection_mutex );
	 }
	END_TRY

	fsd_log_return(( "" ));
}


static void
slurmdrmaa_job_update_status( fsd_job_t *self )
{
	job_info_msg_t *job_info = NULL;
	slurmdrmaa_job_t * slurm_self = (slurmdrmaa_job_t *) self;
	fsd_log_enter(( "({job_id=%s})", self->job_id ));

	fsd_mutex_lock( &self->session->drm_connection_mutex );
	TRY
	{
		if ( slurm_load_job( &job_info, fsd_atoi(self->job_id), SHOW_ALL) ) {
			int _slurm_errno = slurm_get_errno();

			if (_slurm_errno == ESLURM_INVALID_JOB_ID) {
				self->on_missing(self);
			} else {
				fsd_exc_raise_fmt(FSD_ERRNO_INTERNAL_ERROR,"slurm_load_jobs error: %s,job_id: %s", slurm_strerror(slurm_get_errno()), self->job_id);
			}
		}
		if (job_info) {
			fsd_log_debug(("state = %d, state_reason = %d", job_info->job_array[0].job_state, job_info->job_array[0].state_reason));
			
			switch(job_info->job_array[0].job_state & JOB_STATE_BASE)
			{

				case JOB_PENDING:
					switch(job_info->job_array[0].state_reason)
					{
						case WAIT_HELD_USER:   /* job is held by user */
							fsd_log_debug(("interpreting as DRMAA_PS_USER_ON_HOLD"));
							self->state = DRMAA_PS_USER_ON_HOLD;
							break;
						case WAIT_HELD:  /* job is held by administrator */
							fsd_log_debug(("interpreting as DRMAA_PS_SYSTEM_ON_HOLD"));
							self->state = DRMAA_PS_SYSTEM_ON_HOLD;
							break;
						default:
							fsd_log_debug(("interpreting as DRMAA_PS_QUEUED_ACTIVE"));
							self->state = DRMAA_PS_QUEUED_ACTIVE;
					}
					break;
				case JOB_RUNNING:
					fsd_log_debug(("interpreting as DRMAA_PS_RUNNING"));
					self->state = DRMAA_PS_RUNNING;
					break;
				case JOB_SUSPENDED:
					if(slurm_self->user_suspended == true) {
						fsd_log_debug(("interpreting as DRMAA_PS_USER_SUSPENDED"));
						self->state = DRMAA_PS_USER_SUSPENDED;
					} else {
						fsd_log_debug(("interpreting as DRMAA_PS_SYSTEM_SUSPENDED"));
						self->state = DRMAA_PS_SYSTEM_SUSPENDED;
					}
					break;
				case JOB_COMPLETE:
					fsd_log_debug(("interpreting as DRMAA_PS_DONE"));
					self->state = DRMAA_PS_DONE;
					self->exit_status = job_info->job_array[0].exit_code;
					fsd_log_debug(("exit_status = %d -> %d",self->exit_status, WEXITSTATUS(self->exit_status)));
					break;
				case JOB_CANCELLED:
					fsd_log_debug(("interpreting as DRMAA_PS_FAILED (aborted)"));
					self->state = DRMAA_PS_FAILED;
					self->exit_status = -1;
				case JOB_FAILED:
				case JOB_TIMEOUT:
				case JOB_NODE_FAIL:
				case JOB_PREEMPTED:
					fsd_log_debug(("interpreting as DRMAA_PS_FAILED"));
					self->state = DRMAA_PS_FAILED;
					self->exit_status = job_info->job_array[0].exit_code;
					fsd_log_debug(("exit_status = %d -> %d",self->exit_status, WEXITSTATUS(self->exit_status)));
					break;
				default: /*unknown state */
					fsd_log_error(("Unknown job state: %d. Please send bug report: http://apps.man.poznan.pl/trac/slurm-drmaa", job_info->job_array[0].job_state));
			}

			if (job_info->job_array[0].job_state & JOB_STATE_FLAGS & JOB_COMPLETING) {
				fsd_log_debug(("Epilog completing"));
			}

			if (job_info->job_array[0].job_state & JOB_STATE_FLAGS & JOB_CONFIGURING) {
				fsd_log_debug(("Nodes booting"));
			}

			if (self->exit_status == -1) /* input,output,error path failure etc*/
				self->state = DRMAA_PS_FAILED;

			self->last_update_time = time(NULL);
		
			if( self->state >= DRMAA_PS_DONE ) {
				fsd_log_debug(("exit_status = %d, WEXITSTATUS(exit_status) = %d", self->exit_status, WEXITSTATUS(self->exit_status)));
				fsd_cond_broadcast( &self->status_cond );
			}
		}
	}
	FINALLY
	{
		if(job_info != NULL)
			slurm_free_job_info_msg (job_info);

		fsd_mutex_unlock( &self->session->drm_connection_mutex );
	}
	END_TRY
	
	fsd_log_return(( "" ));
}

static void
slurmdrmaa_job_on_missing( fsd_job_t *self )
{

	fsd_log_enter(( "({job_id=%s})", self->job_id ));
	fsd_log_warning(( "Job %s missing from DRM queue", self->job_id ));

	fsd_log_info(( "job_on_missing: last job_ps: %s (0x%02x)", drmaa_job_ps_to_str(self->state), self->state));

	if( self->state >= DRMAA_PS_RUNNING ) { /*if the job ever entered running state assume finished */
		self->state = DRMAA_PS_DONE;
		self->exit_status = 0;
	}
	else {
		self->state = DRMAA_PS_FAILED; /* otherwise failed */
		self->exit_status = -1;
	}

	fsd_log_info(("job_on_missing evaluation result: state=%d exit_status=%d", self->state, self->exit_status));

	fsd_cond_broadcast( &self->status_cond);
	fsd_cond_broadcast( &self->session->wait_condition );

	fsd_log_return(( "; job_ps=%s, exit_status=%d", drmaa_job_ps_to_str(self->state), self->exit_status ));
}

fsd_job_t *
slurmdrmaa_job_new( char *job_id )
{
	slurmdrmaa_job_t *self = NULL;
	self = (slurmdrmaa_job_t*)fsd_job_new( job_id );

	fsd_realloc( self, 1, slurmdrmaa_job_t );

	self->super.control = slurmdrmaa_job_control;
	self->super.update_status = slurmdrmaa_job_update_status;
	self->super.on_missing = slurmdrmaa_job_on_missing;
	self->old_priority = UINT32_MAX;
	self->user_suspended = true;
	return (fsd_job_t*)self;
}


void
slurmdrmaa_job_create_req(
		fsd_drmaa_session_t *session,
		const fsd_template_t *jt,
		fsd_environ_t **envp,
		job_desc_msg_t * job_desc)
{
	fsd_expand_drmaa_ph_t *volatile expand = NULL;

	TRY
	 {
		expand = fsd_expand_drmaa_ph_new( NULL, NULL, fsd_strdup("%a") );
		slurmdrmaa_job_create( session, jt, envp, expand, job_desc );
	 }
	EXCEPT_DEFAULT
	 {
		fsd_exc_reraise();
	 }
	FINALLY
	 {
		if( expand )
			expand->destroy( expand );
	 }
	END_TRY
}

static char *
internal_map_file( fsd_expand_drmaa_ph_t *expand, const char *path,
		bool *host_given, const char *name )
{
	const char *p;

	for( p = path;  *p != ':';  p++ )
		if( *p == '\0' )
			fsd_exc_raise_fmt( FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT,
							"invalid format of drmaa_%s_path: missing colon", name );
	if( host_given )
		*host_given = ( p != path );

	p++;

	return expand->expand( expand, fsd_strdup(p), FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD | FSD_DRMAA_PH_INCR );
}

void
slurmdrmaa_job_create(
		fsd_drmaa_session_t *session,
		const fsd_template_t *jt,
		fsd_environ_t **envp,
		fsd_expand_drmaa_ph_t *expand, 
		job_desc_msg_t * job_desc
		)
{
	const char *input_path_orig = NULL;
	const char *output_path_orig = NULL;
	const char *error_path_orig = NULL;
	char *volatile input_path = NULL;
	char *volatile output_path = NULL;
	char *volatile error_path = NULL;
	bool input_host = false;
	bool output_host = false;
	bool error_host = false;
	bool join_files = false;
	const char *value;
	const char *const *vector;
	const char *job_category = "default";
	
	job_desc->user_id = getuid();
	job_desc->group_id = getgid();

	job_desc->env_size = 0;
	
	/* job name */
	value = jt->get_attr( jt, DRMAA_JOB_NAME );
	if( value )
	{
		job_desc->name = fsd_strdup(value);
		fsd_log_debug(("# job_name = %s",job_desc->name));
	}
	
	/* job state at submit */
	value = jt->get_attr( jt, DRMAA_JS_STATE );
	if( value )
	{
		if( 0 == strcmp( value, DRMAA_SUBMISSION_STATE_ACTIVE ) )
		{}
		else if( 0 == strcmp( value, DRMAA_SUBMISSION_STATE_HOLD ) )
		{
			job_desc->priority = 0;
			fsd_log_debug(("# hold = user"));
		}
		else
		{
			fsd_exc_raise_msg(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE, "invalid value of drmaa_js_state attribute" );
		}
	}
	
	TRY
	{
		const char *command = NULL;
		char *command_expanded = NULL;
		char *temp_script_old = NULL;
		char *temp_script = "";
		const char *const *i;
		int j;

		/* remote command */
		command = jt->get_attr( jt, DRMAA_REMOTE_COMMAND );
		if( command == NULL )
			fsd_exc_raise_msg(
					FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES,
					"drmaa_remote_command not set for job template"
					);

		command_expanded = expand->expand( expand, fsd_strdup(command), FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD );

		temp_script = fsd_asprintf("#!/bin/bash\n%s",command_expanded);
		fsd_free(command_expanded);

		/* arguments list */
		vector = jt->get_v_attr( jt, DRMAA_V_ARGV );

		if( vector )
	 	{
			for( i = vector, j = 2;  *i;  i++, j++ )
			{
				char *arg_expanded = expand->expand( expand, fsd_strdup(*i), FSD_DRMAA_PH_HD | FSD_DRMAA_PH_WD );
				
				temp_script_old = fsd_strdup(temp_script);
				
				if (strcmp(temp_script, "") != 0) {
					fsd_free(temp_script);
				}
				/* add too script */
				temp_script = fsd_asprintf("%s '%s'", temp_script_old, arg_expanded);
				fsd_free(temp_script_old);
				fsd_free(arg_expanded);
			}
		}
		
		job_desc->script = fsd_asprintf("%s\n", temp_script);
		fsd_log_debug(("# Script:\n%s", job_desc->script));
		fsd_free(temp_script);
	}
	END_TRY
	

	/* start time */
	value = jt->get_attr( jt, DRMAA_START_TIME );
	if( value )
 	{ 
		job_desc->begin_time = fsd_datetime_parse( value );
		fsd_log_debug(( "\n  drmaa_start_time: %s -> %ld", value, (long)job_desc->begin_time));
	}

	/*  propagate all environment variables from submission host */
	{
		extern char **environ;
		char **i;
		unsigned j = 0;

		for ( i = environ; *i; i++) {
			job_desc->env_size++;
		}
		
		fsd_log_debug(("environ env_size = %d",job_desc->env_size));
		fsd_calloc(job_desc->environment, job_desc->env_size+1, char *);
		
		for ( i = environ; *i; i++,j++ ) {
			job_desc->environment[j] = fsd_strdup(*i);
		}
	}

	/* environment */
	
	vector = jt->get_v_attr( jt, DRMAA_V_ENV );
	if( vector )
	{
		const char *const *i;
		unsigned j = 0;
		unsigned env_offset = job_desc->env_size;

		for( i = vector;  *i;  i++ )
 		{
			job_desc->env_size++;
		}
		fsd_log_debug(("jt env_size = %d",job_desc->env_size));

		fsd_log_debug(("# environment ="));
		fsd_realloc(job_desc->environment, job_desc->env_size+1, char *);

		for( i = vector;  *i;  i++,j++ )
 		{
			job_desc->environment[j + env_offset] = fsd_strdup(*i);
			fsd_log_debug((" %s", job_desc->environment[j+ env_offset]));
		}
	 }
	
 	/* wall clock time hard limit */
	value = jt->get_attr( jt, DRMAA_WCT_HLIMIT );
	if (value)
	{
		job_desc->time_limit = slurmdrmaa_datetime_parse( value );
		fsd_log_debug(("# wct_hlimit = %s -> %ld",value, (long int)slurmdrmaa_datetime_parse( value )));
	}

		
	/*expand->set(expand, FSD_DRMAA_PH_INCR,fsd_asprintf("%d", n_job));*/ /* set current value */
	/* TODO: test drmaa_ph_incr */
	/* job working directory */
	value = jt->get_attr( jt, DRMAA_WD );
	if( value )
	{
		char *cwd_expanded = expand->expand( expand, fsd_strdup(value), FSD_DRMAA_PH_HD | FSD_DRMAA_PH_INCR );

		expand->set( expand, FSD_DRMAA_PH_WD, fsd_strdup(cwd_expanded));

		fsd_log_debug(("# work_dir = %s",cwd_expanded));
		job_desc->work_dir = fsd_strdup(cwd_expanded);
		fsd_free(cwd_expanded);
	}
	else
	{
		char cwdbuf[4096] = "";

		if ((getcwd(cwdbuf, 4095)) == NULL) {
			char errbuf[256] = "InternalError";
			(void)strerror_r(errno, errbuf, 256); /*on error the default message would be returned */
			fsd_log_error(("getcwd failed: %s", errbuf));
			job_desc->work_dir = fsd_strdup(".");
		} else {
			job_desc->work_dir = fsd_strdup(cwdbuf);
		}

		fsd_log_debug(("work_dir(default:CWD) %s", job_desc->work_dir));
	}

	TRY
 	{
		/* input path */
		input_path_orig = jt->get_attr( jt, DRMAA_INPUT_PATH );
		if( input_path_orig )
		{
			input_path = internal_map_file( expand, input_path_orig, &input_host,"input" );
			fsd_log_debug(( "\n  drmaa_input_path: %s -> %s", input_path_orig, input_path ));
		}

		/* output path */
		output_path_orig = jt->get_attr( jt, DRMAA_OUTPUT_PATH );
		if( output_path_orig )
		{
			output_path = internal_map_file( expand, output_path_orig, &output_host,"output" );
			fsd_log_debug(( "\n  drmaa_output_path: %s -> %s", output_path_orig, output_path ));
		}

		/* error path */
		error_path_orig = jt->get_attr( jt, DRMAA_ERROR_PATH );
		if( error_path_orig )
		{
			error_path = internal_map_file( expand, error_path_orig, &error_host,"error" );
			fsd_log_debug(( "\n  drmaa_error_path: %s -> %s", error_path_orig, error_path ));
		}

		/* join files */
		value = jt->get_attr( jt, DRMAA_JOIN_FILES );
		if( value )
		{
			if( (value[0] == 'y' || value[0] == 'Y')  &&  value[1] == '\0' )
				join_files = true;
			else if( (value[0] == 'n' || value[0] == 'N')  &&  value[1] == '\0' )
				join_files = false;
			else
				fsd_exc_raise_msg(
						FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
						"invalid value of drmaa_join_files attribute" );
		}

		if( join_files )
		{
			if( output_path == NULL )
				fsd_exc_raise_msg(FSD_DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES,	"drmaa_join_files is set and output file is not given" );
			if( error_path!=NULL && 0 != strcmp( output_path, error_path ) )
				fsd_log_warning(( "Error file was given but will be ignored since drmaa_join_files was set." ));

			if (error_path)
				fsd_free(error_path);

			 error_path = fsd_strdup(output_path);
		}
		else
		{
			if( error_path == NULL  &&  output_path )
				error_path = fsd_strdup( "/dev/null" );
			if( output_path == NULL  &&  error_path )
				output_path = fsd_strdup( "/dev/null" );
		}


		/* email addresses to send notifications */
		vector = jt->get_v_attr( jt, DRMAA_V_EMAIL );
		if( vector  &&  vector[0] )
		{
			/* only to one email address message may be send */
			job_desc->mail_user = fsd_strdup(vector[0]);
			job_desc->mail_type = MAIL_JOB_BEGIN | MAIL_JOB_END |  MAIL_JOB_FAIL;
			fsd_log_debug(("# mail_user = %s\n",vector[0]));
			fsd_log_debug(("# mail_type = %o\n",job_desc->mail_type));
			if( vector[1] != NULL )
			{
				fsd_log_error(( "SLURM only supports one e-mail notification address" ));
				fsd_exc_raise_msg(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,"SLURM only supports one e-mail notification address");
			}
		}

		/* block email */
		value = jt->get_attr( jt, DRMAA_BLOCK_EMAIL );
		if( value )
		{
			bool block;
			if( strcmp(value, "0") == 0 )
			{
				block = true;
				fsd_log_debug(("# block_email = true"));
				fsd_log_debug(("# mail_user delated"));
				fsd_free(job_desc->mail_user);
				job_desc->mail_user = NULL;
			}
			else if( strcmp(value, "1") == 0 )
				block = false;
			else
				fsd_exc_raise_msg(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,"invalid value of drmaa_block_email attribute" );

			if( block && output_path == NULL )
			{
				fsd_log_debug(( "output path not set and we want to block e-mail, set to /dev/null" ));
				output_path = fsd_strdup( "/dev/null" );
			}
		}

		if( input_path )
		{
			job_desc->std_in = fsd_strdup(input_path);
			fsd_log_debug(("# input = %s", input_path));
		}

		if( output_path )
		{
			job_desc->std_out = fsd_strdup(output_path);
			fsd_log_debug(("# output = %s", output_path));
		}

		if( error_path )
		{
			job_desc->std_err = fsd_strdup(error_path);
			fsd_log_debug(("# error = %s", error_path));
		}
	 }
	FINALLY
	{
		fsd_free( input_path );
		fsd_free( output_path );
		fsd_free( error_path );
		input_path = NULL;
		output_path = NULL;
		error_path = NULL;
	}
	END_TRY			
	
	
	/* job category */
	value = jt->get_attr( jt, DRMAA_JOB_CATEGORY );
	if( value )
		job_category = value;

	{
		fsd_conf_option_t *category_value = NULL;
		category_value = fsd_conf_dict_get( session->job_categories, job_category );

		if( category_value != NULL )
	 	{
			if( category_value->type != FSD_CONF_STRING )
				fsd_exc_raise_fmt(
						FSD_ERRNO_INTERNAL_ERROR,
						"configuration error: job category should be string"
						);

			fsd_log_debug(("# Job category %s : %s\n",value,category_value->val.string));			
			slurmdrmaa_parse_native(job_desc,category_value->val.string);			
	 	}
		else
	 	{
			if( value != NULL )
				fsd_exc_raise_fmt(
						FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
						"invalid job category: %s", job_category
						);
	 	}
 	}

    /* set defaults for constraints - ref: slurm.h */
    fsd_log_debug(("# Setting defaults for tasks and processors" ));
    job_desc->num_tasks = 1;
    job_desc->min_cpus = 0;
    job_desc->cpus_per_task = 0;
    job_desc->pn_min_cpus = 0;

	/* native specification */
	value = jt->get_attr( jt, DRMAA_NATIVE_SPECIFICATION );
	if( value )
	{
		fsd_log_debug(("# Native specification: %s\n", value));
		slurmdrmaa_parse_native(job_desc, value);
	}
	
}


