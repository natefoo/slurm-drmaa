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
