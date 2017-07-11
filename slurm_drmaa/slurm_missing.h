/* $Id: $ */
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

#ifndef __LL_DRMAA__SLURM_MISSING_H
#define __LL_DRMAA__SLURM_MISSING_H

extern void * slurm_list_peek (List l);
extern void * slurm_list_remove (ListIterator i);

/* --clusters is not supported with Slurm < 15.08, but these are defined to
 * avoid compiler warnings
 */
#if SLURM_VERSION_NUMBER < SLURM_VERSION_NUM(14,11,0)
extern List slurmdb_get_info_cluster(char *cluster_name);
extern void * slurm_list_pop (List l);
extern void * slurm_list_peek (List l);
extern void * slurm_list_remove (ListIterator i);
extern void slurmdb_destroy_cluster_rec(void *object);
#endif

#endif /* __SLURM_DRMAA__SLURM_MISSING_H */
