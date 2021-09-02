# source

SLURM_TEST_DIR=_slurm

function setup() {
    mkdir -p "$SLURM_TEST_DIR"
}

function slurm_check() {
    rm -f _slurm_available
    run ./slurm_ping
    [ $status -eq 0 ] && touch _slurm_available
}

function slurm_available() {
    [ -f _slurm_available ]
}

function slurm_config_value() {
    command -v scontrol >/dev/null || skip "Cannot get config value $1: scontrol not found"
    scontrol show config | awk -F= "\$1 ~ /^$1 *$/ {print \$2}" | xargs
}

function slurm_version_major() {
    command -v scontrol >/dev/null || skip "Cannot get Slurm version: scontrol not found"
    scontrol version | awk '{print $NF}' | awk -F. '{print $1 $2}'
}

function _slurm_test_prefix() {
    local base dir
    base=$(basename "$BATS_TEST_FILENAME" .bats)
    dir="$(pwd)/$SLURM_TEST_DIR"
    echo "${dir}/${base}-${BATS_TEST_NUMBER}-${BATS_TEST_NAME}"
}

function _write_slurm_script() {
    printf "#!/bin/sh\n$2\n" >"$1"
    chmod +x "$1"
}

function drmaa_run() {
    local prefix
    local opts=()
    [ "${1:0:1}" = '-' ] && { opts+=("-native=$1"); shift; }
    prefix=$(_slurm_test_prefix)
    opts+=("-stdout=${prefix}.stdout")
    opts+=("-stderr=${prefix}.stderr")
    _write_slurm_script "${prefix}.sh" "${1:-echo OK}"
    rm -f "$prefix".std{out,err}
    DRMAA_LIBRARY_PATH=../slurm_drmaa/.libs/libdrmaa.so run \
        ../drmaa_utils/drmaa_utils/drmaa-run "${opts[@]}" "'${prefix}.sh'" </dev/null
}

function drmaa_job_ps() {
    local jobid="$1"
    DRMAA_LIBRARY_PATH=../slurm_drmaa/.libs/libdrmaa.so run \
        ../drmaa_utils/drmaa_utils/drmaa-job-ps "$jobid"
}
