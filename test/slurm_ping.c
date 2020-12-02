#include <slurm/slurm.h>

/* Slurm doesn't provide a public method to just use the local config, which we need in order to set a timeout without
 * having to contact slurmctld first... */
extern int slurm_conf_destroy(void);
extern slurm_conf_t *slurm_conf_lock(void);
extern void slurm_conf_unlock(void);

int main(int argc, char **argv) {
	int status = 1;

	slurm_conf_t *slurm_ctl_conf_ptr = slurm_conf_lock();
	slurm_ctl_conf_ptr->msg_timeout = 3;
	slurm_conf_unlock();

#if SLURM_VERSION_NUMBER < SLURM_VERSION_NUM(18,8,0)
	for (uint32_t i = 1; i < 2; i++) {
#else
	for (uint32_t i = 0; i < slurm_ctl_conf_ptr->control_cnt; i++) {
#endif
		printf("slurm_ping(%i) == %i\n", i, slurm_ping(i));
		if (slurm_ping(i) == SLURM_SUCCESS) {
			status = 0;
			break;
		}
	}

	slurm_conf_destroy();
	return status;
}
