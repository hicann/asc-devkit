#!/usr/bin/env bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${PROJECT_ROOT:-$(cd "${SCRIPT_DIR}/.." && pwd)}"

ASCEND_HOME_DIR="${ASCEND_HOME_DIR:-/usr/local/Ascend/ascend-toolkit/latest}"
ARCH="${ARCH:-dav-2201}"
MODES="${MODES:-${MODE:-npu}}"
SCHEDULE="${SCHEDULE:-fixed}"
SCHEDULE_REPORT="${SCHEDULE_REPORT:-}"
SCHEDULE_FILE="${SCHEDULE_FILE:-}"
STRICT_SCHEDULE="${STRICT_SCHEDULE:-1}"
JOBS="${JOBS:-auto}"
NPU_SLOTS="${NPU_SLOTS:-1}"
CPU_RUN_SLOTS="${CPU_RUN_SLOTS:-auto}"
MAKE_JOBS="${MAKE_JOBS:-auto}"
TIMEOUT="${TIMEOUT:-120}"
CPU_RUN_TIMEOUT="${CPU_RUN_TIMEOUT:-300}"
PRIMARY_CARD="${PRIMARY_CARD:-0}"
RETRY_CARDS="${RETRY_CARDS:-}"
NPU_CARDS="${NPU_CARDS:-auto}"
NPU_CARD_DEV_GLOB="${NPU_CARD_DEV_GLOB:-/dev/davinci[0-9]*}"
REPORT_FORMAT="${REPORT_FORMAT:-all}"
PRESMOKE_WERROR="${PRESMOKE_WERROR:-0}"
OUT_ROOT="${OUT_ROOT:-${PROJECT_ROOT}/presmoke_reports/presmoke_${ARCH}_$(date +%Y%m%d_%H%M%S)}"
LOCK_DIR="${LOCK_DIR:-${PROJECT_ROOT}/.presmoke_locks/run_presmoke.lock}"
PRESMOKE_SHARED_STATE_DIR="${PRESMOKE_SHARED_STATE_DIR:-${OUT_ROOT}/.state}"

if [[ "$MODES" != "npu" && "${SCHEDULE:-}" == "fixed" && -z "$SCHEDULE_FILE" ]]; then
    SCHEDULE="default"
fi

mkdir -p "$OUT_ROOT"
STATUS_FILE="$OUT_ROOT/status.txt"

date_iso() {
    date '+%Y-%m-%dT%H:%M:%S%z'
}

log() {
    printf '[%s] %s\n' "$(date_iso)" "$*" | tee -a "$STATUS_FILE"
}

cleanup_lock() {
    rm -rf "$LOCK_DIR"
}

cleanup_transient_run_artifacts() {
    local found=0
    [[ -d "$OUT_ROOT/.plan" ]] && found=1
    [[ -d "$OUT_ROOT/.state" ]] && found=1
    (( found == 1 )) || return 0

    log "transient_artifact_cleanup_start root=$OUT_ROOT"
    rm -rf -- "$OUT_ROOT/.plan" "$OUT_ROOT/.state"
    log "transient_artifact_cleanup_done root=$OUT_ROOT"
}

cleanup_runtime() {
    cleanup_transient_run_artifacts
    cleanup_lock
}

acquire_lock() {
    mkdir -p "$(dirname "$LOCK_DIR")"
    if mkdir "$LOCK_DIR" 2>/dev/null; then
        echo "$$" > "$LOCK_DIR/pid"
        trap cleanup_runtime EXIT
        return
    fi

    local pid
    pid="$(cat "$LOCK_DIR/pid" 2>/dev/null || true)"
    if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
        echo "Error: presmoke is already running, pid=$pid lock=$LOCK_DIR" >&2
        exit 2
    fi
    rm -rf "$LOCK_DIR"
    mkdir "$LOCK_DIR"
    echo "$$" > "$LOCK_DIR/pid"
    trap cleanup_runtime EXIT
}

source_cann() {
    set +u
    if [[ -f "${ASCEND_HOME_DIR}/set_env.sh" ]]; then
        # shellcheck disable=SC1090
        source "${ASCEND_HOME_DIR}/set_env.sh" 2>/dev/null || true
    elif [[ -f "/usr/local/Ascend/cann/set_env.sh" ]]; then
        # shellcheck disable=SC1091
        source "/usr/local/Ascend/cann/set_env.sh" 2>/dev/null || true
    fi
    set -u
}

run_python_presmoke() {
    source_cann
    export PYTHONPATH="${SCRIPT_DIR}${PYTHONPATH:+:${PYTHONPATH}}"
    export PRESMOKE_PROJECT_ROOT="${PROJECT_ROOT}"
    exec python3 -m presmoke "$@"
}

run_python_module() {
    source_cann
    PYTHONPATH="${SCRIPT_DIR}${PYTHONPATH:+:${PYTHONPATH}}" \
    PRESMOKE_PROJECT_ROOT="${PROJECT_ROOT}" \
    python3 -m "$@"
}

npu_smi_info() {
    if ! command -v npu-smi >/dev/null 2>&1; then
        return 0
    fi
    if command -v timeout >/dev/null 2>&1; then
        timeout 10s npu-smi info 2>/dev/null || true
        return 0
    fi

    npu-smi info 2>/dev/null &
    local pid=$!
    local waited=0
    while kill -0 "$pid" 2>/dev/null; do
        if (( waited >= 10 )); then
            kill "$pid" 2>/dev/null || true
            return 0
        fi
        sleep 1
        waited=$((waited + 1))
    done
    wait "$pid" 2>/dev/null || true
}

presmoke_opp_path() {
    if [[ -n "${ASCEND_OPP_PATH:-}" ]]; then
        printf '%s\n' "$ASCEND_OPP_PATH"
        return
    fi
    if [[ -n "${ASCEND_HOME_PATH:-}" && -d "$ASCEND_HOME_PATH/opp" ]]; then
        printf '%s\n' "$ASCEND_HOME_PATH/opp"
        return
    fi
    printf '%s\n' "/usr/local/Ascend/cann-9.1.0/opp"
}

clean_cann_vendors() {
    source_cann
    local vendors_dir
    vendors_dir="$(presmoke_opp_path)/vendors"
    if [[ ! -d "$vendors_dir" ]]; then
        log "vendors_clean_skip missing_dir=$vendors_dir"
        return
    fi
    log "vendors_clean_begin dir=$vendors_dir"
    find "$vendors_dir" -mindepth 1 -maxdepth 1 -exec rm -rf {} +
    log "vendors_clean_done dir=$vendors_dir"
}

should_clean_cann_vendors() {
    local arg
    for arg in "$@"; do
        case "$arg" in
            --dry-run|--filter|--filter=*|--exact-filter|--exact-filter=*) return 1 ;;
        esac
    done
    return 0
}

truthy() {
    case "${1:-}" in
        1|true|TRUE|yes|YES|on|ON) return 0 ;;
        *) return 1 ;;
    esac
}

should_strict_schedule() {
    local arg
    [[ "$SCHEDULE" == "fixed" ]] || return 1
    truthy "$STRICT_SCHEDULE" || return 1
    for arg in "$@"; do
        case "$arg" in
            --filter|--filter=*|--exact-filter|--exact-filter=*|--exclude|--exclude=*) return 1 ;;
        esac
    done
    return 0
}

detect_npu_cards() {
    local dev base id
    for dev in $NPU_CARD_DEV_GLOB; do
        [[ -e "$dev" ]] || continue
        base="$(basename "$dev")"
        id="${base#davinci}"
        [[ "$id" =~ ^[0-9]+$ ]] || continue
        printf '%s\n' "$id"
    done | sort -n -u
}

detect_npu_cards_by_smi() {
    if ! command -v npu-smi >/dev/null 2>&1; then
        return 0
    fi
    npu_smi_info | awk '
        {
            gsub(/\|/, " ")
            if ($1 ~ /^[0-9]+$/) {
                print $1
            }
        }
    ' | sort -n -u
}

normalize_card_list() {
    local raw="$1"
    raw="${raw//,/ }"
    local card
    for card in $raw; do
        [[ -n "$card" ]] && printf '%s\n' "$card"
    done
}

detect_cpu_count() {
    if command -v lscpu >/dev/null 2>&1; then
        lscpu | awk -F: '/^CPU\(s\):/ {gsub(/^[ \t]+|[ \t]+$/, "", $2); print $2; exit}'
        return
    fi
    getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1
}

resolve_shard_parallel_value() {
    local value="$1"
    local resolved="$2"
    if [[ "$value" == "auto" ]]; then
        echo "$resolved"
    else
        echo "$value"
    fi
}

resolve_multi_card_shard_parallel() {
    local card_count="$1"
    local cpu_count per_card_cpus shard_jobs shard_make_jobs shard_cpu_run_slots
    cpu_count="$(detect_cpu_count)"
    per_card_cpus=$(( (cpu_count + card_count - 1) / card_count ))
    if (( per_card_cpus < 1 )); then
        per_card_cpus=1
    fi
    # Keep several independent builds feeding each card while earlier cases run.
    # Most samples have few translation units, so large make -j values do not use
    # the CPU budget efficiently. Budget four CPUs per build, which keeps
    # shard_make_jobs at 4 on every runner, and cap the build count to limit
    # compiler memory pressure.
    # 每个 build 分 4 个 CPU 是两台 CI runner 实测值的共同解：
    #   A2  192 核 / 8 卡 -> per_card_cpus=24 -> jobs=6、make_jobs=4
    #   A5  256 核 / 4 卡 -> per_card_cpus=64 -> jobs=16、make_jobs=4
    # 上限 16 在这两台上都不生效（A5 恰好等于 16），只用来兜住更大的机器 ——
    # 16 也是目前实测过的最大值。
    shard_jobs=$(( per_card_cpus / 4 ))
    if (( shard_jobs < 1 )); then
        shard_jobs=1
    elif (( shard_jobs > 16 )); then
        shard_jobs=16
    fi
    shard_jobs="$(resolve_shard_parallel_value "$JOBS" "$shard_jobs")"
    # JOBS 可能是 0/非数字，下面拿它做除数，不兜住会在 set -e 下炸掉整个 presmoke。
    if ! [[ "$shard_jobs" =~ ^[0-9]+$ ]] || (( shard_jobs < 1 )); then
        shard_jobs=1
    fi
    shard_make_jobs=$(( per_card_cpus / shard_jobs ))
    if (( shard_make_jobs < 1 )); then
        shard_make_jobs=1
    fi
    shard_cpu_run_slots="$per_card_cpus"
    printf '%s\t%s\t%s\t%s\n' \
        "$shard_jobs" \
        "$(resolve_shard_parallel_value "$MAKE_JOBS" "$shard_make_jobs")" \
        "$(resolve_shard_parallel_value "$CPU_RUN_SLOTS" "$shard_cpu_run_slots")" \
        "$per_card_cpus"
}

resolve_npu_cards() {
    if [[ "$NPU_CARDS" == "auto" ]]; then
        local detected
        detected="$(detect_npu_cards)"
        if [[ -z "$detected" ]]; then
            detected="$(detect_npu_cards_by_smi)"
        fi
        if [[ -n "$detected" ]]; then
            printf '%s\n' "$detected" | awk '{print NR - 1}'
        else
            printf '%s\n' "$PRIMARY_CARD"
        fi
        return
    fi
    normalize_card_list "$NPU_CARDS"
}

has_arg_requiring_single_run() {
    local arg stage_value=""
    for arg in "$@"; do
        if [[ -n "$stage_value" ]]; then
            [[ "$arg" == "build" ]] && return 0
            stage_value=""
            continue
        fi
        case "$arg" in
            --dry-run|--filter|--filter=*|--exact-filter|--exact-filter=*|--exclude|--exclude=*)
                return 0
                ;;
            --stages) stage_value="pending" ;;
            --stages=build) return 0 ;;
        esac
    done
    return 1
}

should_run_multi_card() {
    local card_count="$1"
    shift
    [[ "$MODES" == "npu" ]] || return 1
    [[ -n "$NPU_CARDS" ]] || return 1
    (( card_count > 1 )) || return 1
    ! has_arg_requiring_single_run "$@"
}

plan_examples_for_sharding() {
    local plan_dir="$OUT_ROOT/.plan"
    rm -rf "$plan_dir"
    mkdir -p "$plan_dir/results"
    (
        cd "$PROJECT_ROOT"
        export ASCEND_HOME_DIR
        export PRESMOKE_WERROR
        args=(
            --arch "$ARCH"
            --modes "$MODES"
            --jobs "$JOBS"
            --npu-slots "$NPU_SLOTS"
            --cpu-run-slots "$CPU_RUN_SLOTS"
            --make-jobs "$MAKE_JOBS"
            --timeout "$TIMEOUT"
            --cpu-run-timeout "$CPU_RUN_TIMEOUT"
            --schedule "$SCHEDULE"
            --report-format "$REPORT_FORMAT"
            --results "$plan_dir/results"
            --dry-run
        )
        if [[ -n "$SCHEDULE_FILE" ]]; then
            args+=(--schedule-file "$SCHEDULE_FILE")
        fi
        if [[ -n "$SCHEDULE_REPORT" ]]; then
            args+=(--schedule-report "$SCHEDULE_REPORT")
        fi
        if should_strict_schedule "$@"; then
            args+=(--strict-schedule)
        fi
        if truthy "$PRESMOKE_WERROR"; then
            args+=(--werror)
        fi
        run_python_presmoke "${args[@]}" "$@"
    ) > "$plan_dir/stdout.log" 2> "$plan_dir/stderr.log"
    run_python_module presmoke.orchestrate_report list-examples "$plan_dir/results/report.json"
}

write_multi_card_shards() {
    local shards_dir="$1"
    shift
    local cards=("$@")
    local assignments="$OUT_ROOT/.plan/shards.tsv"
    local shard_args=(
        presmoke.orchestrate_report
        shard-examples
        "$OUT_ROOT/.plan/results/report.json"
    )
    if [[ -n "$SCHEDULE_REPORT" ]]; then
        shard_args+=(--schedule-report "$SCHEDULE_REPORT")
    fi
    if [[ "${MULTI_CARD_SHARD_JOBS:-}" =~ ^[0-9]+$ ]]; then
        shard_args+=(--jobs "$MULTI_CARD_SHARD_JOBS")
    elif [[ "$JOBS" =~ ^[0-9]+$ ]]; then
        shard_args+=(--jobs "$JOBS")
    fi
    local fixed_shards_dir
    fixed_shards_dir="${SCRIPT_DIR}/presmoke/schedules/shards/${ARCH}_${MODES}_${#cards[@]}cards"
    if [[ -z "$SCHEDULE_REPORT" && -d "$fixed_shards_dir" ]]; then
        log "multi_card_fixed_shards dir=$fixed_shards_dir"
        shard_args+=(--fixed-shards "$fixed_shards_dir")
    fi
    shard_args+=(--cards "${cards[@]}")

    rm -rf "$shards_dir"
    mkdir -p "$shards_dir"
    run_python_module "${shard_args[@]}" > "$assignments"

    local card
    while IFS=$'\t' read -r card example; do
        [[ -n "$card" && -n "$example" ]] || continue
        printf '%s\n' "$example" >> "$shards_dir/card_${card}.txt"
    done < "$assignments"
}

run_multi_card_presmoke() {
    local cards_csv="$1"
    shift
    local cards=()
    local examples=()
    local card example
    while IFS= read -r card; do
        [[ -n "$card" ]] && cards+=("$card")
    done <<< "$cards_csv"
    while IFS= read -r example; do
        [[ -n "$example" ]] || continue
        examples+=("$example")
    done < <(plan_examples_for_sharding "$@")
    if [[ "${#examples[@]}" -eq 0 ]]; then
        log "multi_card_skip no_planned_cases"
        local fallback_card="${cards[0]:-$PRIMARY_CARD}"
        run_presmoke "full_card${fallback_card}" "$fallback_card" "$@"
        return
    fi

    log "multi_card_start cards=${cards[*]} cases=${#examples[@]}"
    local shard_parallel shard_jobs shard_make_jobs shard_cpu_run_slots shard_per_card_cpus
    shard_parallel="$(resolve_multi_card_shard_parallel "${#cards[@]}")"
    IFS=$'\t' read -r shard_jobs shard_make_jobs shard_cpu_run_slots shard_per_card_cpus <<< "$shard_parallel"
    log "multi_card_shard_parallel per_card_cpus=$shard_per_card_cpus" \
        "jobs=$shard_jobs make_jobs=$shard_make_jobs cpu_run_slots=$shard_cpu_run_slots"
    local shards_dir="$OUT_ROOT/.plan/shards"
    MULTI_CARD_SHARD_JOBS="$shard_jobs" write_multi_card_shards "$shards_dir" "${cards[@]}"

    local index shard_count pid rc
    local pids=()
    for index in "${!cards[@]}"; do
        card="${cards[$index]}"
        shard_count=0
        local shard_args=()
        local shard_file="$shards_dir/card_${card}.txt"
        if [[ -f "$shard_file" ]]; then
            while IFS= read -r example; do
                [[ -n "$example" ]] || continue
                shard_args+=(--exact-filter "$example")
                shard_count=$((shard_count + 1))
            done < "$shard_file"
        fi
        if (( shard_count == 0 )); then
            log "multi_card_shard_skip card=$card count=0"
            continue
        fi
        log "multi_card_shard_start card=$card count=$shard_count"
        (
            export RUN_PRESMOKE_JOBS="$shard_jobs"
            export RUN_PRESMOKE_MAKE_JOBS="$shard_make_jobs"
            export RUN_PRESMOKE_CPU_RUN_SLOTS="$shard_cpu_run_slots"
            run_presmoke "full_card${card}" "$card" "${shard_args[@]}" "$@"
        ) &
        pids+=("$!")
    done

    rc=0
    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            rc=1
        fi
    done
    log "multi_card_done rc=$rc"
    return 0
}

run_presmoke() {
    local name="$1"
    local card="$2"
    shift 2
    local run_jobs="${RUN_PRESMOKE_JOBS:-$JOBS}"
    local run_make_jobs="${RUN_PRESMOKE_MAKE_JOBS:-$MAKE_JOBS}"
    local run_cpu_run_slots="${RUN_PRESMOKE_CPU_RUN_SLOTS:-$CPU_RUN_SLOTS}"
    local run_dir="$OUT_ROOT/$name"
    mkdir -p "$run_dir"
    log "run_start name=$name card=$card arch=$ARCH modes=$MODES jobs=$run_jobs" \
        "npu_slots=$NPU_SLOTS cpu_run_slots=$run_cpu_run_slots make_jobs=$run_make_jobs" \
        "timeout=$TIMEOUT cpu_run_timeout=$CPU_RUN_TIMEOUT schedule=$SCHEDULE" \
        "werror=$PRESMOKE_WERROR args=$*"
    {
        echo "name=$name"
        echo "started_at=$(date_iso)"
        echo "project_root=$PROJECT_ROOT"
        echo "git=$(cd "$PROJECT_ROOT" && git rev-parse --short HEAD 2>/dev/null || true)"
        echo "arch=$ARCH"
        echo "modes=$MODES"
        echo "schedule=$SCHEDULE"
        echo "schedule_report=$SCHEDULE_REPORT"
        echo "schedule_file=$SCHEDULE_FILE"
        echo "strict_schedule=$STRICT_SCHEDULE"
        echo "jobs=$run_jobs"
        echo "npu_slots=$NPU_SLOTS"
        echo "cpu_run_slots=$run_cpu_run_slots"
        echo "make_jobs=$run_make_jobs"
        echo "timeout=$TIMEOUT"
        echo "cpu_run_timeout=$CPU_RUN_TIMEOUT"
        echo "werror=$PRESMOKE_WERROR"
        echo "card=$card"
        echo "extra_args=$*"
        source_cann
        echo "npu_smi_before_begin"
        npu_smi_info
        echo "npu_smi_before_end"
    } > "$run_dir/meta.txt"

    set +e
    local start end rc
    start="$(date +%s)"
    (
        cd "$PROJECT_ROOT"
        export ASCEND_HOME_DIR
        export ASCEND_RT_VISIBLE_DEVICES="$card"
        export ASCEND_VISIBLE_DEVICES="$card"
        export ASCEND_DEVICE_ID=0
        export NPU_DEVICE_ID=0
        export PRESMOKE_WERROR
        export PRESMOKE_STATE_DIR="$PRESMOKE_SHARED_STATE_DIR"
        args=(
            --arch "$ARCH"
            --modes "$MODES"
            --jobs "$run_jobs"
            --npu-slots "$NPU_SLOTS"
            --cpu-run-slots "$run_cpu_run_slots"
            --make-jobs "$run_make_jobs"
            --timeout "$TIMEOUT"
            --cpu-run-timeout "$CPU_RUN_TIMEOUT"
            --schedule "$SCHEDULE"
            --report-format "$REPORT_FORMAT"
            --results "$run_dir/results"
        )
        if [[ -n "$SCHEDULE_FILE" ]]; then
            args+=(--schedule-file "$SCHEDULE_FILE")
        fi
        if [[ -n "$SCHEDULE_REPORT" ]]; then
            args+=(--schedule-report "$SCHEDULE_REPORT")
        fi
        if should_strict_schedule "$@"; then
            args+=(--strict-schedule)
        fi
        if truthy "$PRESMOKE_WERROR"; then
            args+=(--werror)
        fi
        run_python_presmoke "${args[@]}" "$@"
    ) > "$run_dir/stdout.log" 2> "$run_dir/stderr.log"
    rc=$?
    end="$(date +%s)"
    set -e

    {
        echo "finished_at=$(date_iso)"
        echo "rc=$rc"
        echo "elapsed_sec=$((end - start))"
        source_cann
        echo "npu_smi_after_begin"
        npu_smi_info
        echo "npu_smi_after_end"
    } >> "$run_dir/meta.txt"
    log "run_done name=$name card=$card rc=$rc elapsed_sec=$((end - start))"
    return 0
}

timeout_examples() {
    local report="$1"
    [[ -f "$report" ]] || return
    run_python_module presmoke.orchestrate_report timeout-examples "$report"
}

retry_timeouts() {
    local full_reports=()
    local full_report
    while IFS= read -r full_report; do
        [[ -f "$full_report" ]] && full_reports+=("$full_report")
    done < <(find "$OUT_ROOT" -mindepth 3 -maxdepth 3 -path '*/full_card*/results/report.json' | sort)
    [[ "${#full_reports[@]}" -gt 0 ]] || return
    local examples=()
    local example
    for full_report in "${full_reports[@]}"; do
        while IFS= read -r example; do
            examples+=("$example")
        done < <(timeout_examples "$full_report")
    done
    if [[ "${#examples[@]}" -eq 0 ]]; then
        log "retry_skip no_timeouts"
        return
    fi
    if [[ -z "$RETRY_CARDS" ]]; then
        log "retry_skip retry_cards_empty count=${#examples[@]}"
        return
    fi

    log "retry_timeouts count=${#examples[@]}"
    local example card safe_name retry_report status
    for example in "${examples[@]}"; do
        [[ -n "$example" ]] || continue
        safe_name="${example//\//__}"
        for card in $RETRY_CARDS; do
            log "retry_start example=$example card=$card"
            run_presmoke "retry_${safe_name}_card${card}" "$card" --exact-filter "$example"
            retry_report="$OUT_ROOT/retry_${safe_name}_card${card}/results/report.json"
            status="$(run_python_module presmoke.orchestrate_report retry-status "$retry_report")"
            log "retry_done example=$example card=$card status=$status"
            [[ "$status" == "PASS" ]] && break
        done
    done
}

write_final_report() {
    run_python_module presmoke.orchestrate_report write-final-report "$OUT_ROOT"
    log "final_report_written $OUT_ROOT/FINAL_REPORT.md"
}

main() {
    # 先做静态对齐检查：examples 下有「编译运行」段却没有 run.sh 的样例，对 strict-schedule
    # 和分片校验都是不存在的，会静默漏跑。这个检查不到 1s，放最前面，省掉十几分钟后才报错。
    if ! run_python_module presmoke.check_case_coverage; then
        log "presmoke_abort reason=case_coverage_mismatch"
        return 1
    fi
    acquire_lock
    log "presmoke_start out_root=$OUT_ROOT"
    if should_clean_cann_vendors "$@"; then
        clean_cann_vendors
    else
        log "vendors_clean_skip reason=non_full_run args=$*"
    fi
    local cards_csv
    local cards=()
    local card
    cards_csv="$(resolve_npu_cards)"
    while IFS= read -r card; do
        [[ -n "$card" ]] && cards+=("$card")
    done <<< "$cards_csv"
    if should_run_multi_card "${#cards[@]}" "$@"; then
        # 默认 fixed：shared 当默认会让 test_orchestrate 里 4 个断言多卡分片行为的用例
        # 直接失败（它们等的是 multi_card_start / multi_card_fixed_shards / full_cardN
        # 目录）。shared 还没有任何一轮 CI 数据，先当显式 opt-in。
        if [[ "${PRESMOKE_MULTI_CARD_MODE:-fixed}" == "fixed" ]]; then
            run_multi_card_presmoke "$cards_csv" "$@"
        else
            # 复用 fixed 的每卡预算再乘卡数，两种模式的 CPU 占用才可比：fixed 是每卡一个
            # 子跑、各开 per_card_jobs 个 build worker，shared 只有一个进程，所以要
            # jobs = per_card_jobs x 卡数、make_jobs 仍按每卡算。
            # 原来的 (cpus+1)/2 在 256 核上会算出 jobs=128、make_jobs=2 —— build worker
            # 比 NPU 槽多一个数量级，而 make -j2 又把单个 build 拖慢。
            local jobs make_jobs shard_parallel per_card_jobs
            shard_parallel="$(resolve_multi_card_shard_parallel "${#cards[@]}")"
            IFS=$'\t' read -r per_card_jobs make_jobs _ _ <<< "$shard_parallel"
            jobs=$(( per_card_jobs * ${#cards[@]} ))
            (( jobs >= 1 )) || jobs=1
            (( make_jobs >= 1 )) || make_jobs=1
            log "multi_card_shared cards=${cards[*]} jobs=$jobs make_jobs=$make_jobs per_card_jobs=$per_card_jobs"
            PRESMOKE_SHARED_CARDS="${cards[*]}" RUN_PRESMOKE_JOBS="$jobs" \
                RUN_PRESMOKE_MAKE_JOBS="$make_jobs" run_presmoke "full_card${cards[0]}" "${cards[0]}" "$@"
        fi
    else
        card="${cards[0]:-$PRIMARY_CARD}"
        run_presmoke "full_card${card}" "$card" "$@"
    fi
    retry_timeouts
    write_final_report
    cleanup_transient_run_artifacts
    local effective_rc
    effective_rc="$(cat "$OUT_ROOT/effective_rc.txt" 2>/dev/null || echo 1)"
    log "presmoke_done out_root=$OUT_ROOT effective_rc=$effective_rc"
    run_python_module presmoke.orchestrate_report print-summary "$OUT_ROOT"
    if [[ "$effective_rc" -eq 0 ]]; then
        echo "execute samples success"
    else
        echo "execute samples failed" >&2
    fi
    exit "$effective_rc"
}

main "$@"
