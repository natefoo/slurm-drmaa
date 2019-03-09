/*
 * DRMAA for Slurm
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

#ifndef __LL_DRMAA__SLURM_DRMAA_H
#define __LL_DRMAA__SLURM_DRMAA_H

typedef struct {
	char *original;
	char *job_id;
	char *cluster;
} job_id_spec_t;

#endif /* __SLURM_DRMAA__SLURM_DRMAA_H */
