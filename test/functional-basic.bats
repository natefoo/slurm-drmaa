#!/usr/bin/env bats

load helper_slurm

@test "check slurm" {
    run slurm_check
}

@test "run job with no parameters" {
    slurm_available || skip "Slurm unreachable"
    drmaa_run
    [ "$status" -eq 0 ]
    [ "$output" = "OK" ]
}

@test "run job with common short native params" {
    slurm_available || skip "Slurm unreachable"
    drmaa_run '-N 1 -n 1'
    [ "$status" -eq 0 ]
    [ "$output" = "OK" ]
}

@test "run job with common long native params" {
    slurm_available || skip "Slurm unreachable"
    drmaa_run '--nodes=1 --ntasks=1'
    [ "$status" -eq 0 ]
    [ "$output" = "OK" ]
}

@test "ensure that submit umask is passed" {
    slurm_available || skip "Slurm unreachable"
    umask 027
    drmaa_run umask
    [ "$status" -eq 0 ]
    [ "$output" -eq 27 ]
    umask 022
    drmaa_run umask
    [ "$status" -eq 0 ]
    [ "$output" -eq 22 ]
}

@test "ensure that submit host and dir variables are passed" {
    slurm_available || skip "Slurm unreachable"
    drmaa_run 'echo "${SLURM_SUBMIT_HOST}:${SLURM_SUBMIT_DIR}"'
    [ "$status" -eq 0 ]
    [ "$output" = "$(hostname):$(pwd)" ]
}

@test "ensure that process priority is passed" {
    slurm_available || skip "Slurm unreachable"
    drmaa_run nice
    [ "$status" -eq 0 ]
    [ "$output" -eq 0 ]
    renice -n 5 $$ >/dev/null
    drmaa_run 'echo "$SLURM_PRIO_PROCESS"'
    [ "$status" -eq 0 ]
    [ "$output" -eq 5 ]
}

@test "verify that status is lost after MinJobAge without \$SLURM_DRMAA_USE_SLURMDBD" {
    slurm_available || skip "Slurm unreachable"
    slurm_config_value "MinJobAge" || skip "Cannot get config value MinJobAge"
    local min_job_age=${output/ sec/}
    drmaa_run 'echo "$SLURM_JOB_ID"'
    [ "$status" -eq 0 ]
    local job_id="$output"
    drmaa_job_ps "$job_id"
    [ "$status" -eq 0 ]
    [[ "$output" == *done ]]
    sleep $((min_job_age + 10))
    drmaa_job_ps "$job_id"
    [[ "$output" == *failed ]]
}

@test "verify that status is NOT lost after MinJobAge with \$SLURM_DRMAA_USE_SLURMDBD" {
    slurm_available || skip "Slurm unreachable"
    slurm_config_value "MinJobAge" || skip "Cannot get config value MinJobAge"
    local min_job_age=${output/ sec/}
    drmaa_run 'echo "$SLURM_JOB_ID"'
    [ "$status" -eq 0 ]
    local job_id="$output"
    SLURM_DRMAA_USE_SLURMDBD=1 drmaa_job_ps "$job_id"
    [ "$status" -eq 0 ]
    [[ "$output" == *done ]]
    sleep $((min_job_age + 10))
    SLURM_DRMAA_USE_SLURMDBD=1 drmaa_job_ps "$job_id"
    [[ "$output" == *done ]]
}
