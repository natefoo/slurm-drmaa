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

#include <drmaa_utils/common.h>
#include <drmaa_utils/exception.h>
#include <slurm_drmaa/util.h>
#include <string.h>

#include <time.h>
#include <drmaa_utils/datetime.h>
#include <drmaa_utils/datetime_impl.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id$";
#endif

unsigned int
slurmdrmaa_datetime_parse( const char *string )
{
#ifdef DEBUGGING
	char dbg[256];
#endif
	fsd_dt_parser_t *volatile parser = NULL;
	fsd_dt_lexer_t *volatile lexer = NULL;
	int parse_err = 0;
	fsd_datetime_t dt;

	memset(&dt, 0, sizeof(fsd_datetime_t));

	fsd_log_enter(( "(%s)", string ));
	TRY
	 {
		fsd_malloc( parser, fsd_dt_parser_t );
		fsd_malloc( lexer, fsd_dt_lexer_t );

		parser->lexer = lexer;
		parser->n_errors = 0;
		lexer->parser = parser;
		lexer->begin = lexer->p = (unsigned char*)string;
		lexer->end = (unsigned char*)( string + strlen(string) );
		parse_err = fsd_dt_parse( parser, lexer );
		if( parse_err || parser->n_errors )
			fsd_exc_raise_fmt(
					FSD_ERRNO_INVALID_VALUE_FORMAT,
					"invalid date/time format: %s", string
					);

		dt = parser->result;
#ifdef DEBUGGING
		fsd_datetime_dump( &dt, dbg, sizeof(dbg) );
		fsd_log_debug(( "parsed: %s", dbg ));
#endif
	}
	FINALLY
	 {
		fsd_free( parser );
		fsd_free( lexer );
	 }
	END_TRY

	fsd_log_return(( "(%s) =%u minutes", string, (unsigned)60*dt.hour+dt.minute ));
	return 60*dt.hour+dt.minute;
}

/* taken from SLURM src/common/proc_args.c */
static int slurmdrmaa_mail_type_parse(const char *mail_type_str)
{
	int rc = 0;


	if (strcasecmp(mail_type_str, "BEGIN") == 0)
		rc = MAIL_JOB_BEGIN;
	else if (strcasecmp(mail_type_str, "END") == 0)
		rc = MAIL_JOB_END;
	else if (strcasecmp(mail_type_str, "FAIL") == 0)
		rc = MAIL_JOB_FAIL;
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(2,2,0)
	else if (strcasecmp(mail_type_str, "REQUEUE") == 0)
		rc = MAIL_JOB_REQUEUE;
#endif
	else if (strcasecmp(mail_type_str, "ALL") == 0)
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(2,2,0)
		rc = MAIL_JOB_BEGIN | MAIL_JOB_END | MAIL_JOB_FAIL | MAIL_JOB_REQUEUE;
#else
		rc = MAIL_JOB_BEGIN | MAIL_JOB_END | MAIL_JOB_FAIL;
#endif
	else {
		fsd_log_error(("Unknown mail type: %s", mail_type_str));
	}

	return rc;
}


enum slurm_native {
	SLURM_NATIVE_ACCOUNT,
	SLURM_NATIVE_ACCTG_FREQ,
	SLURM_NATIVE_COMMENT,
	SLURM_NATIVE_CONSTRAINT,
	SLURM_NATIVE_CONTIGUOUS,
	SLURM_NATIVE_EXCLUSIVE,
	SLURM_NATIVE_MEM,
	SLURM_NATIVE_MEM_PER_CPU,
	SLURM_NATIVE_MINCPUS,
	SLURM_NATIVE_NODELIST,
	SLURM_NATIVE_NODES,
	SLURM_NATIVE_NTASKS_PER_NODE,
	SLURM_NATIVE_PARTITION,
	SLURM_NATIVE_QOS,
	SLURM_NATIVE_REQUEUE,
	SLURM_NATIVE_RESERVATION,
	SLURM_NATIVE_SHARE,
	SLURM_NATIVE_JOB_NAME,
	SLURM_NATIVE_TIME_LIMIT,
	SLURM_NATIVE_NTASKS,
	SLURM_NATIVE_GRES,
	SLURM_NATIVE_NO_KILL,
	SLURM_NATIVE_LICENSES,
	SLURM_NATIVE_MAIL_TYPE,
	SLURM_NATIVE_NO_REQUEUE,
	SLURM_NATIVE_EXCLUDE,
	SLURM_NATIVE_TMP
};

void
slurmdrmaa_init_job_desc(job_desc_msg_t *job_desc)
{
	memset(job_desc, 0, sizeof(job_desc_msg_t));
}

void
slurmdrmaa_free_job_desc(job_desc_msg_t *job_desc)
{
	unsigned i = 0;
	fsd_log_enter(( "" ));
	fsd_free(job_desc->account);	
	fsd_free(job_desc->comment);
	
	for(i = 0;i<job_desc->env_size;i++)
	{
		fsd_free(job_desc->environment[i]);
	}	
	fsd_free(job_desc->environment);
	fsd_free(job_desc->features);
	fsd_free(job_desc->name);	
	fsd_free(job_desc->mail_user);
	fsd_free(job_desc->partition);
	fsd_free(job_desc->qos);
	fsd_free(job_desc->script);
	fsd_free(job_desc->std_in);
	fsd_free(job_desc->std_out);
	fsd_free(job_desc->std_err);	
	fsd_free(job_desc->work_dir);
	fsd_free(job_desc->exc_nodes);
#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(2,2,0)
	fsd_free(job_desc->gres);
#endif
	
	fsd_log_return(( "" ));
}

void
slurmdrmaa_add_attribute(job_desc_msg_t *job_desc, unsigned attr, const char *value)
{
	char * ptr = NULL;
	char * rest = NULL;
	char * token = NULL;

	switch(attr)
	{
		case SLURM_NATIVE_ACCOUNT:
			fsd_free(job_desc->account);
			fsd_log_debug(("# account = %s",value));
			job_desc->account = fsd_strdup(value);
			break;
		case SLURM_NATIVE_ACCTG_FREQ:
			fsd_log_debug(("# acctg_freq = %s",value));
			job_desc->acctg_freq = fsd_atoi(value);
			break;
		case SLURM_NATIVE_COMMENT:
			fsd_free(job_desc->comment);
			fsd_log_debug(("# comment = %s",value));
			job_desc->comment = fsd_strdup(value);
			break;
		case SLURM_NATIVE_CONSTRAINT:
			fsd_free(job_desc->features);
			fsd_log_debug(("# constraints -> features = %s",value));
			job_desc->features = fsd_strdup(value);
			break;
		case SLURM_NATIVE_CONTIGUOUS:
			fsd_log_debug(( "# contiguous = 1"));
			job_desc->contiguous = 1;
			break;
		case SLURM_NATIVE_EXCLUSIVE:
			fsd_log_debug(( "# exclusive -> shared = 0"));
			job_desc->shared = 0;
			break;
		case SLURM_NATIVE_MEM:
		#if SLURM_VERSION_NUMBER < SLURM_VERSION_NUM(2,2,0)
			if(job_desc->job_min_memory == NO_VAL ||  fsd_atoi(value) > (int)job_desc->job_min_memory) {
				fsd_log_debug(("# job_min_memory = %s",value));
				job_desc->job_min_memory = fsd_atoi(value);
			}
		#else
			if(job_desc->pn_min_memory == NO_VAL ||  fsd_atoi(value) > (int)job_desc->pn_min_memory) {
				fsd_log_debug(("# pn_min_memory = %s",value));
				job_desc->pn_min_memory = fsd_atoi(value);
			}
		#endif
			else { 
				fsd_log_debug(("mem value defined lower or equal to mem-per-cpu or value defined before"));
			}
			break;
		case SLURM_NATIVE_MEM_PER_CPU:
		#if SLURM_VERSION_NUMBER < SLURM_VERSION_NUM(2,2,0)
			if(job_desc->job_min_memory == NO_VAL ||  fsd_atoi(value) > (int)job_desc->job_min_memory) {
				fsd_log_debug(("# job_min_memory = %s",value));
				job_desc->job_min_memory = fsd_atoi(value);
			}
		#else
			if(job_desc->pn_min_memory == NO_VAL ||  fsd_atoi(value) > (int)job_desc->pn_min_memory) {
				fsd_log_debug(("# pn_min_memory = %s",value));
				job_desc->pn_min_memory = fsd_atoi(value);
			}
		#endif
			else { 
				fsd_log_debug(("mem-per-cpu value defined lower or equal to mem or value defined before"));
			}
			break;
		case SLURM_NATIVE_MINCPUS:
		#if SLURM_VERSION_NUMBER < SLURM_VERSION_NUM(2,2,0)
			fsd_log_debug(("# job_min_cpus = %s",value));
			job_desc->job_min_cpus = fsd_atoi(value);
		#else
			fsd_log_debug(("# min_cpus = %s",value));
			job_desc->min_cpus = fsd_atoi(value);
		#endif
			break;
		case SLURM_NATIVE_NODELIST:
			fsd_free(job_desc->req_nodes);
			fsd_log_debug(("# node-list - > req_nodes = %s",value));
			job_desc->req_nodes = fsd_strdup(value);
			break;
		case SLURM_NATIVE_NODES:
			ptr = strdup(value);
			
			if((token = strtok_r(ptr,"=",&rest)) == NULL){
				fsd_log_error(("strtok_r returned NULL"));
			}
						
			fsd_log_debug(("nodes: %s ->",value));
			fsd_log_debug(("# min_nodes = %s",token));
			job_desc->min_nodes = fsd_atoi(token);
						
			if(strcmp(rest,"") !=0 ) {
				fsd_log_debug(("# max_nodes = %s",rest));
				job_desc->max_nodes = fsd_atoi(rest);
			}
			fsd_free(ptr);
			break;		
		case SLURM_NATIVE_NTASKS_PER_NODE:
			fsd_log_debug(("# ntasks_per_node = %s",value));
			job_desc->ntasks_per_node = fsd_atoi(value);
			break;
		case SLURM_NATIVE_PARTITION:
			fsd_free(job_desc->partition);
			fsd_log_debug(("# partition = %s",value));
			job_desc->partition = fsd_strdup(value);
			break;
		case SLURM_NATIVE_QOS:
			fsd_free(job_desc->qos);
			fsd_log_debug(("# qos = %s",value));
			job_desc->qos = fsd_strdup(value);
			break;
		case SLURM_NATIVE_REQUEUE:
			fsd_log_debug(( "# requeue = 1"));
			job_desc->requeue = 1;
			break;
		case SLURM_NATIVE_RESERVATION:
			fsd_log_debug(("# reservation = %s",value));
			job_desc->reservation = fsd_strdup(value);
			break;
		case SLURM_NATIVE_SHARE:
			fsd_log_debug(("# shared = 1"));
			job_desc->shared = 1;
			break;
		case SLURM_NATIVE_JOB_NAME:
			fsd_log_debug(("# job_name = %s",job_desc->name));
			job_desc->name = fsd_strdup(value);
			break;
		case SLURM_NATIVE_NTASKS:
			fsd_log_debug(("# ntasks = %s",value));
			job_desc->num_tasks = fsd_atoi(value); 
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_MINCPUS,value);
			break;	
		case SLURM_NATIVE_TIME_LIMIT:
			fsd_log_debug(("# time_limit = %s",value));
			job_desc->time_limit = slurmdrmaa_datetime_parse(value); 
			break;	
		case SLURM_NATIVE_GRES:
			#if SLURM_VERSION_NUMBER >= SLURM_VERSION_NUM(2,2,0)
			fsd_log_debug(("# gres = %s",value));
			job_desc->gres = fsd_strdup(value);
			#else
			fsd_log_error(("GRES not supported in this version of SLURM."));
			#endif
			break;
		case SLURM_NATIVE_NO_KILL:
			fsd_log_debug(("# no_kill = 1"));
			job_desc->kill_on_node_fail = 0;
			break;
		case SLURM_NATIVE_LICENSES:
			fsd_log_debug(("# licenses = %s", value));
			job_desc->licenses = fsd_strdup(value);
			break;
		case SLURM_NATIVE_MAIL_TYPE:
			fsd_log_debug(("# mail_type = %s", value));
			job_desc->mail_type = slurmdrmaa_mail_type_parse(value);
			break;
		case SLURM_NATIVE_NO_REQUEUE:
			fsd_log_debug(("# requeue = 0"));
			job_desc->requeue = 0;
			break;
		case SLURM_NATIVE_EXCLUDE:
			fsd_log_debug(("# exclude = %s", value));
			job_desc->exc_nodes = fsd_strdup(value);
			break;
		case SLURM_NATIVE_TMP:
			fsd_log_debug(("# tmp = %s", value));
			job_desc->job_min_tmp_disk = fsd_atoi(value);
			break;
	
		default:
			fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,"Invalid attribute");
	}
}

void 
slurmdrmaa_parse_additional_attr(job_desc_msg_t *job_desc,const char *add_attr)
{
	char * volatile name = NULL;
	char *value = NULL;
	char *ctxt = NULL;
	char * volatile add_attr_copy = fsd_strdup(add_attr);

	TRY
	  {
		name = fsd_strdup(strtok_r(add_attr_copy, "=", &ctxt));
		value = strtok_r(NULL, "=", &ctxt);
		/*
		 * TODO: move it to slurmdrmaa_add_attribute
		 if (value == NULL) {
			fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE, 
				"Invalid native specification: %s Missing '='.", add_attr_copy);
		} */

		if(strcmp(name,"account") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_ACCOUNT,value);
		}
		else if(strcmp(name,"acctg-freq") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_ACCTG_FREQ,value);
		}
		else if (strcmp(name,"comment") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_COMMENT,value);
		}
		else if (strcmp(name,"constraint") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_CONSTRAINT,value);
		}
		else if (strcmp(name,"contiguous") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_CONTIGUOUS,NULL);
		}
		else if(strcmp(name,"exclusive") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_EXCLUSIVE,NULL);
		}
		else if (strcmp(name,"mem") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_MEM,value);
		}
		else if (strcmp(name,"mem-per-cpu") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_MEM_PER_CPU,value);
		}
		else if (strcmp(name,"mincpus") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_MINCPUS,value);
		}
		else if (strcmp(name,"nodelist") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NODELIST,value);			
		}
		else if (strcmp(name,"nodes") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NODES,value);
		}
		else if (strcmp(name,"ntasks-per-node") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NTASKS_PER_NODE,value);
		}
		else if (strcmp(name,"partition") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_PARTITION,value);
		}
		else if (strcmp(name,"qos") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_QOS,value);
		}
		else if (strcmp(name,"requeue") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_REQUEUE,NULL);
		}
		else if (strcmp(name,"reservation") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_RESERVATION,value);
		}
		else if (strcmp(name,"share") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_SHARE,NULL);
		}		
		else if(strcmp(name,"job_name") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_JOB_NAME,value);
		}
		else if(strcmp(name,"time_limit") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_TIME_LIMIT,value);
		}
		else if(strcmp(name,"time") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_TIME_LIMIT,value);
		}
		else if(strcmp(name,"ntasks") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NTASKS,value);
		}
		else if(strcmp(name,"gres") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_GRES,value);
		}
		else if(strcmp(name,"no-kill") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NO_KILL,NULL);
		}
		else if(strcmp(name,"licenses") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_LICENSES,value);
		}
		else if(strcmp(name,"mail-type") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_MAIL_TYPE,value);
		}
		else if(strcmp(name,"no-requeue") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NO_REQUEUE,NULL);
		}
		else if(strcmp(name,"exclude") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_EXCLUDE,value);
		}
		else if(strcmp(name,"tmp") == 0) {
			slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_TMP,value);
		} else {
			fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
					"Invalid native specification: %s (Unsupported option: --%s)",
					add_attr, name);
		}
	
	  }
	FINALLY
	  {
		fsd_free(name);
		fsd_free(add_attr_copy);
	  }
	END_TRY
}

void 
slurmdrmaa_parse_native(job_desc_msg_t *job_desc, const char * value)
{
	char *arg = NULL;
	char * volatile native_specification = fsd_strdup(value);
	char * volatile native_spec_copy = fsd_strdup(native_specification);
	char * ctxt = NULL;
	int opt = 0;
		
	fsd_log_enter(( "" ));
	TRY
	 {
		for (arg = strtok_r(native_spec_copy, " \t", &ctxt); arg; arg = strtok_r(NULL, " \t",&ctxt) ) {
			if (!opt) {
				if ( (arg[0] != '-') || ((strlen(arg) != 2) && (strlen(arg) > 2) && arg[2] != ' ' && arg[1] !='-' ) ) {
					fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
							"Invalid native specification: %s",
							native_specification);
				}
				if(arg[1] == '-') {
					slurmdrmaa_parse_additional_attr(job_desc, arg+2);
				}
				else {
					opt = arg[1];
				}		
			} else {
				switch (opt) {
					case 'A' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_ACCOUNT, arg);
						break;
					case 'C' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_CONSTRAINT, arg);
						break;	
					case 'N' :	
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NODES, arg);
						break;	
					case 'p' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_PARTITION, arg);
						break;
					case 's' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_SHARE, NULL);
						break;
					case 'w' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NODELIST, arg);
						break;
					case 'J' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_JOB_NAME, arg);
						break;
					case 't' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_TIME_LIMIT, arg);
						break;  
					case 'n' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_NTASKS, arg);
						break;  
					case 'x' :
						slurmdrmaa_add_attribute(job_desc,SLURM_NATIVE_EXCLUDE, arg);
						break;
					default :
							fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
									"Invalid native specification: %s (Unsupported option: -%c)",
									native_specification, opt);
				}
				if (arg[0] == '-' && strlen(arg) == 2) { /* attribute without arg */
					opt = arg[1];
				}
				else {
					opt = 0;
				}
			}
		}

		if(strlen(native_spec_copy) == 2 && native_spec_copy[0] == '-' && native_spec_copy[1] == 's')
		{
			slurmdrmaa_add_attribute(job_desc, SLURM_NATIVE_SHARE, NULL);
			opt = 0;
		}
		
		if (opt)
			fsd_exc_raise_fmt(FSD_DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE,
					"Invalid native specification: %s",
					native_specification);

	 }
	FINALLY
	 {
		fsd_free(native_spec_copy);
		fsd_free(native_specification);
	 }
	END_TRY

	fsd_log_return(( "" ));
}

