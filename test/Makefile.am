TESTS = functional-basic.bats
TEST_EXTENSIONS = .bats
BATS_LOG_COMPILER = bats

check_PROGRAMS = slurm_ping

slurm_ping_CPPFLAGS = @SLURM_INCLUDES@
slurm_ping_LDFLAGS = @SLURM_LDFLAGS@
slurm_ping_LDADD = @SLURM_LIBS@

clean-local:
	rm -rf _slurm
	rm -f _slurm_available
