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

#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <drmaa_utils/common.h>
#include <drmaa_utils/compat.h>
#include <drmaa_utils/drmaa.h>
#include <drmaa_utils/drmaa_base.h>
#include <drmaa_utils/iter.h>
#include <drmaa_utils/drmaa_attrib.h>

#include <slurm_drmaa/session.h>
#include <slurm/slurm.h>

static char slurmdrmaa_version[50] = "";

static fsd_drmaa_session_t *
slurmdrmaa_new_session( fsd_drmaa_singletone_t *self, const char *contact )
{
	return slurmdrmaa_session_new( contact );
}

static fsd_template_t *
slurmdrmaa_new_job_template( fsd_drmaa_singletone_t *self )
{
	return drmaa_template_new();
}

static const char *
slurmdrmaa_get_contact( fsd_drmaa_singletone_t *self )
{
	return "";
}

static void
slurmdrmaa_get_version( fsd_drmaa_singletone_t *self,
		unsigned *major, unsigned *minor )
{
	*major = 1;  *minor = 0;
}

static const char *
slurmdrmaa_get_DRM_system( fsd_drmaa_singletone_t *self )
{
	if(slurmdrmaa_version[0] == '\0') /*no locks as drmaa_get_drm_system is usually called only once */
	{
		slurm_ctl_conf_t * conf_info_msg_ptr = NULL; 
		if ( slurm_load_ctl_conf ((time_t) NULL, &conf_info_msg_ptr ) == -1 ) 
		{ 
			fsd_log_error(("slurm_load_ctl_conf error: %s",slurm_strerror(slurm_get_errno())));
			fsd_snprintf(NULL, slurmdrmaa_version, sizeof(slurmdrmaa_version)-1,"SLURM");
		}
		else
		{
			fsd_snprintf(NULL, slurmdrmaa_version, sizeof(slurmdrmaa_version)-1,"SLURM %s", conf_info_msg_ptr->version);
			slurm_free_ctl_conf (conf_info_msg_ptr);
		}
	}
	return slurmdrmaa_version;
}

static const char *
slurmdrmaa_get_DRMAA_implementation( fsd_drmaa_singletone_t *self )
{
	return PACKAGE_STRING;
}


fsd_iter_t *
slurmdrmaa_get_attribute_names( fsd_drmaa_singletone_t *self )
{
	static const char *attribute_names[] = {
		DRMAA_REMOTE_COMMAND,
		DRMAA_JS_STATE,
		DRMAA_WD,
		DRMAA_JOB_CATEGORY,
		DRMAA_NATIVE_SPECIFICATION,
		DRMAA_BLOCK_EMAIL,
		DRMAA_START_TIME,
		DRMAA_JOB_NAME,
		DRMAA_INPUT_PATH,
		DRMAA_OUTPUT_PATH,
		DRMAA_ERROR_PATH,
		DRMAA_JOIN_FILES,
		DRMAA_WCT_HLIMIT,
		NULL
	};
	return fsd_iter_new_const( attribute_names, -1 );
}

fsd_iter_t *
slurmdrmaa_get_vector_attribute_names( fsd_drmaa_singletone_t *self )
{
	static const char *attribute_names[] = {
		DRMAA_V_ARGV,
		DRMAA_V_ENV,
		DRMAA_V_EMAIL,
		NULL
	};
	return fsd_iter_new_const( attribute_names, -1 );
}

static int
slurmdrmaa_wifexited(
		int *exited, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	/*bool run;
	int exit_status;
	run = ((stat & 0xff) == 0);
	exit_status = ((stat & 0xff00) >> 8) & 0xff;
	*exited = run && (exit_status <= 128);*/
	*exited = WIFEXITED(stat);
	return DRMAA_ERRNO_SUCCESS;
}

static int
slurmdrmaa_wexitstatus(
		int *exit_status, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	/**exit_status = ((stat & 0xff00) >> 8) & 0xff;*/
	*exit_status = WEXITSTATUS(stat);
	return DRMAA_ERRNO_SUCCESS;
}

static int
slurmdrmaa_wifsignaled(
		int *signaled, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*signaled = ((stat & 0xff00) >> 8) >= 128;
	/**signaled = WIFSIGNALED(stat);*/
	return DRMAA_ERRNO_SUCCESS;
}

static int
slurmdrmaa_wtermsig(
		char *signal, size_t signal_len, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	int sig = ((stat & 0xff00) >> 8) - 128;
	strlcpy( signal, fsd_strsignal(sig), signal_len );
	/*int sig = WTERMSIG(stat);
	strlcpy( signal, fsd_strsignal(sig), signal_len );*/
	return DRMAA_ERRNO_SUCCESS;
}

static int
slurmdrmaa_wcoredump(
		int *core_dumped, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	/**core_dumped = 0;*/
	*core_dumped = ((stat)&0200); 
	return DRMAA_ERRNO_SUCCESS;
}

static int
slurmdrmaa_wifaborted(
		int *aborted, int stat,
		char *error_diagnosis, size_t error_diag_len
		)
{
	*aborted = ((stat & 0x01) != 0);
	return DRMAA_ERRNO_SUCCESS;
}


fsd_drmaa_singletone_t _fsd_drmaa_singletone = {
	NULL,
	FSD_MUTEX_INITIALIZER,

	slurmdrmaa_new_session,
	slurmdrmaa_new_job_template,

	slurmdrmaa_get_contact,
	slurmdrmaa_get_version,
	slurmdrmaa_get_DRM_system,
	slurmdrmaa_get_DRMAA_implementation,

	slurmdrmaa_get_attribute_names,
	slurmdrmaa_get_vector_attribute_names,

	slurmdrmaa_wifexited,
	slurmdrmaa_wexitstatus,
	slurmdrmaa_wifsignaled,
	slurmdrmaa_wtermsig,
	slurmdrmaa_wcoredump,
	slurmdrmaa_wifaborted
};
