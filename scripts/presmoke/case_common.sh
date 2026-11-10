#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

presmoke_project_root() {
    local root="${PRESMOKE_PROJECT_ROOT:-}"
    if [[ -z "$root" ]]; then
        root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
    fi
    printf '%s\n' "$root"
}

presmoke_case_root() {
    local rel="$1"
    local root
    root="$(presmoke_project_root)"
    printf '%s/examples/%s\n' "$root" "$rel"
}

presmoke_arch() {
    printf '%s\n' "${PRESMOKE_ARCH:-dav-2201}"
}

presmoke_mode() {
    printf '%s\n' "${PRESMOKE_MODE:-npu}"
}

presmoke_werror_enabled() {
    case "${PRESMOKE_WERROR:-}" in
        1|true|TRUE|yes|YES|on|ON) return 0 ;;
        *) return 1 ;;
    esac
}

presmoke_cmake_is_configure_command() {
    local arg
    for arg in "$@"; do
        case "$arg" in
            --build|--install|--open|--workflow|-E) return 1 ;;
        esac
    done
    return 0
}

presmoke_cmake_has_cache_arg() {
    local key="$1"
    shift
    local arg
    for arg in "$@"; do
        [[ "$arg" == "-D${key}="* ]] && return 0
    done
    return 1
}

presmoke_werror_cmake_args() {
    presmoke_werror_enabled || return 0
    printf '%s\n' "-DCMAKE_ASC_FLAGS=-Werror"
}

presmoke_append_werror_flag() {
    local value="${1:-}"
    if [[ -z "$value" ]]; then
        printf '%s\n' "-Werror"
        return
    fi
    case " $value " in
        *" -Werror "*) printf '%s\n' "$value" ;;
        *) printf '%s\n' "$value -Werror" ;;
    esac
}

cmake() {
    local extra=()
    if presmoke_werror_enabled && presmoke_cmake_is_configure_command "$@"; then
        presmoke_cmake_has_cache_arg "CMAKE_ASC_FLAGS" "$@" || extra+=("-DCMAKE_ASC_FLAGS=-Werror")
        CFLAGS="$(presmoke_append_werror_flag "${CFLAGS:-}")" \
        CXXFLAGS="$(presmoke_append_werror_flag "${CXXFLAGS:-}")" \
        command cmake "$@" "${extra[@]}"
        return
    fi
    command cmake "$@" "${extra[@]}"
}

export -f presmoke_werror_enabled
export -f presmoke_cmake_is_configure_command
export -f presmoke_cmake_has_cache_arg
export -f presmoke_append_werror_flag
export -f cmake

presmoke_soc_version() {
    if [[ -n "${PRESMOKE_SOC_VERSION:-}" ]]; then
        printf '%s\n' "$PRESMOKE_SOC_VERSION"
        return
    fi
    case "${1:-$(presmoke_arch)}" in
        dav-2201) printf '%s\n' "Ascend910B4" ;;
        dav-3510) printf '%s\n' "Ascend950PR_9599" ;;
        *) printf '%s\n' "${1:-$(presmoke_arch)}" ;;
    esac
}

presmoke_build_dir() {
    local case_dir="$1"
    printf '%s/build_%s\n' "$case_dir" "$(presmoke_mode)"
}

presmoke_case_init() {
    if [[ $# -ne 1 || -z "${1:-}" ]]; then
        echo "Usage: presmoke_case_init <case_rel>" >&2
        return 2
    fi

    CASE_DIR="$(presmoke_case_root "$1")"
    MODE="$(presmoke_mode)"
    ARCH="$(presmoke_arch)"
    SOC_VERSION="$(presmoke_soc_version "$ARCH")"
    RUN_MODE_ARG=""
    if [[ "$MODE" != "npu" ]]; then
        RUN_MODE_ARG="-DCMAKE_ASC_RUN_MODE=$MODE"
    fi
    BUILD_DIR="$(presmoke_build_dir "$CASE_DIR")"
    TORCH_DEVICE_BACKEND_AUTOLOAD="${TORCH_DEVICE_BACKEND_AUTOLOAD:-0}"
    ASCEND_SLOG_PRINT_TO_STDOUT="${PRESMOKE_ASCEND_SLOG_PRINT_TO_STDOUT:-0}"
    export CASE_DIR MODE ARCH SOC_VERSION RUN_MODE_ARG BUILD_DIR
    export TORCH_DEVICE_BACKEND_AUTOLOAD
    export ASCEND_SLOG_PRINT_TO_STDOUT
}

run_in_build_dir() {
    (cd "$BUILD_DIR" && "$@")
}

presmoke_default_clean() {
    rm -rf "$BUILD_DIR"
}

# SKIP_ARCHS 为空表示该 skip 不限 arch；非空时只在列出的 arch 上生效。
# 一个 run.sh 服务所有 arch，所以 arch 限定只能这样在运行时判。
presmoke_skip_applies_to_arch() {
    if [[ -z "${SKIP_ARCHS:-}" ]]; then
        return 0
    fi
    local current_arch="${ARCH:-$(presmoke_arch)}"
    local skip_arch
    for skip_arch in "${SKIP_ARCHS[@]}"; do
        if [[ "$current_arch" == "$skip_arch" ]]; then
            return 0
        fi
    done
    return 1
}

presmoke_case_main() {
    local action="${1:-all}"

    if [[ "$action" == "clean" ]]; then
        case_clean
        return
    fi

    if [[ -n "${SKIP_REASON:-}" ]] && presmoke_skip_applies_to_arch; then
        if [[ -z "${SKIP_MODES:-}" ]]; then
            echo "SKIP: $SKIP_REASON"
            return 0
        fi
        local current_mode="${MODE:-$(presmoke_mode)}"
        for skip_mode in "${SKIP_MODES[@]}"; do
            if [[ "$current_mode" == "$skip_mode" ]]; then
                echo "SKIP: $SKIP_REASON (mode=$current_mode)"
                return 0
            fi
        done
    fi

    case "$action" in
        build) case_build ;;
        run) case_run ;;
        verify) case_verify ;;
        all) case_build; case_run; case_verify ;;
        *) echo "Usage: $0 {build|run|verify|clean|all}" >&2; return 2 ;;
    esac
}

presmoke_run_command() {
    echo "+ $*" >&2
    "$@"
}

presmoke_ascend_log_root() {
    printf '%s\n' "${PRESMOKE_ASCEND_LOG_ROOT:-$HOME/ascend/log}"
}

presmoke_verify_tiling_sink_task_log_for_pid() {
    local pid="$1"
    local pattern="$2"
    local timeout="${PRESMOKE_PLOG_FLUSH_TIMEOUT:-10}"
    local log_root deadline plog_dir plog_file
    local -a plog_files=()

    if [[ ! "$pid" =~ ^[0-9]+$ ]]; then
        echo "invalid tiling sink process id: $pid" >&2
        return 1
    fi

    log_root="$(presmoke_ascend_log_root)"
    deadline=$((SECONDS + timeout))
    while true; do
        plog_files=()
        for plog_dir in "$log_root/debug/plog" "$log_root/run/plog"; do
            [[ -d "$plog_dir" ]] || continue
            while IFS= read -r -d '' plog_file; do
                plog_files+=("$plog_file")
            done < <(find "$plog_dir" -maxdepth 1 -type f -name "plog-${pid}_*.log" -print0 2>/dev/null)
        done
        if ((${#plog_files[@]} > 0)) && grep -F -n -- "$pattern" "${plog_files[@]}" >&2; then
            echo "tiling sink task generation log matched for pid $pid" >&2
            return 0
        fi
        ((SECONDS >= deadline)) && break
        sleep 0.2
    done

    echo "tiling sink task generation log not found for pid $pid: $pattern" >&2
    if ((${#plog_files[@]} == 0)); then
        echo "no PID-specific plog found under: $log_root" >&2
    else
        printf 'searched plog file: %s\n' "${plog_files[@]}" >&2
    fi
    return 1
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

presmoke_custom_op_package_valid() {
    local vendor_dir
    vendor_dir="$(presmoke_opp_path)/vendors/customize"
    [[ -f "$vendor_dir/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.py" ]] || return 1
    [[ -f "$vendor_dir/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom_tiling_sink.py" ]] || return 1
    [[ -f "$vendor_dir/op_api/include/aclnn_add_custom.h" ]] || return 1
    [[ -f "$vendor_dir/op_api/include/aclnn_add_custom_tiling_sink.h" ]] || return 1
    [[ -f "$vendor_dir/op_api/lib/libcust_opapi.so" ]] || return 1
    [[ -f "$vendor_dir/framework/onnx/libcust_onnx_parsers.so" ]] || return 1
    [[ -f "$vendor_dir/op_impl/ai_core/tbe/op_master_device/lib/libcust_opmaster.so" ]] || return 1
}

presmoke_custom_op_dynamic_dirs() {
    local opp_path
    opp_path="$(presmoke_opp_path)"
    printf '%s\n' "$opp_path/vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic"
    printf '%s\n' "$opp_path/vendors/add_custom/op_impl/ai_core/tbe/add_custom_impl/dynamic"
}

presmoke_repair_custom_op_package_headers() {
    local build_dir="$1"
    local vendor_dynamic source_dynamic header
    source_dynamic="$build_dir/op_kernel/ascendc_kernels/binary/dynamic"
    [[ -d "$source_dynamic" ]] || return 0

    while IFS= read -r vendor_dynamic; do
        [[ -d "$vendor_dynamic" ]] || continue
        while IFS= read -r header; do
            cp "$header" "$vendor_dynamic/"
        done < <(find "$source_dynamic" -mindepth 2 -maxdepth 2 -type f \( -name '*.h' -o -name '*.hpp' \) 2>/dev/null)
    done < <(presmoke_custom_op_dynamic_dirs)
}

presmoke_custom_op_lock_dir() {
    local lock_root opp_path lock_key
    lock_root="${PRESMOKE_LOCK_ROOT:-/tmp/asc-devkit-presmoke-locks}"
    opp_path="$(presmoke_opp_path)"
    if command -v sha256sum >/dev/null 2>&1; then
        lock_key="$(printf '%s' "$opp_path" | sha256sum | awk '{print $1}')"
    elif command -v shasum >/dev/null 2>&1; then
        lock_key="$(printf '%s' "$opp_path" | shasum -a 256 | awk '{print $1}')"
    else
        lock_key="$(printf '%s' "$opp_path" | cksum | awk '{print $1}')"
    fi
    printf '%s/%s/custom_op_package.lock\n' "$lock_root" "$lock_key"
}

presmoke_with_directory_lock() {
    local lock_dir="$1"
    shift
    local lock_pid rc
    mkdir -p "$(dirname "$lock_dir")"

    while ! mkdir "$lock_dir" 2>/dev/null; do
        lock_pid="$(cat "$lock_dir/pid" 2>/dev/null || true)"
        if [[ "$lock_pid" =~ ^[0-9]+$ ]] && ! kill -0 "$lock_pid" 2>/dev/null; then
            rm -rf "$lock_dir"
            continue
        fi
        sleep 0.1
    done

    printf '%s\n' "${BASHPID:-$$}" > "$lock_dir/pid"
    rc=0
    "$@" || rc=$?
    rm -rf "$lock_dir"
    return "$rc"
}

presmoke_ensure_custom_op_package() {
    if [[ "${PRESMOKE_CUSTOM_OP_AUTO_INSTALL:-1}" == "0" ]]; then
        echo "PRESMOKE_CUSTOM_OP_AUTO_INSTALL=0; custom_op package auto-install skipped" >&2
        return 0
    fi
    presmoke_with_directory_lock \
        "$(presmoke_custom_op_lock_dir)" _presmoke_ensure_custom_op_package
}

_presmoke_ensure_custom_op_package() {
    local root custom_case_dir build_dir state_dir stamp arch mode package
    root="$(presmoke_project_root)"
    custom_case_dir="$root/examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
    if [[ ! -d "$custom_case_dir" ]]; then
        echo "custom_op case directory not found: $custom_case_dir" >&2
        return 2
    fi

    arch="$(presmoke_arch)"
    mode="$(presmoke_mode)"
    build_dir="$(presmoke_build_dir "$custom_case_dir")"
    state_dir="${PRESMOKE_STATE_DIR:-$root/.presmoke_state}"
    stamp="$state_dir/custom_op_${arch}_${mode}.installed"
    mkdir -p "$build_dir" "$state_dir"

    if [[ -f "$stamp" ]]; then
        presmoke_repair_custom_op_package_headers "$build_dir"
        if presmoke_custom_op_package_valid; then
            echo "custom_op package already installed for arch=$arch mode=$mode" >&2
            return 0
        fi
        echo "custom_op package stamp exists but installed files are incomplete; reinstalling" >&2
        rm -f "$stamp"
    fi

    local cmake_args=(cmake ..)
    if [[ -n "${NLOHMANN_JSON_URL:-}" ]]; then
        cmake_args+=("-DNLOHMANN_JSON_URL=$NLOHMANN_JSON_URL")
    fi
    if [[ "$mode" != "npu" ]]; then
        cmake_args+=("-DCMAKE_ASC_RUN_MODE=$mode")
    fi

    (cd "$build_dir" && presmoke_run_command "${cmake_args[@]}")
    (
        cd "$build_dir"
        PRESMOKE_MAKE_JOBS=16 CMAKE_BUILD_PARALLEL_LEVEL=16 \
            presmoke_run_command make -j binary package
    )

    local packages=("$build_dir"/custom_opp_*.run)
    if [[ ! -f "${packages[0]}" ]]; then
        echo "custom_op package was not generated under $build_dir" >&2
        return 1
    fi
    package="$(basename "${packages[0]}")"
    (cd "$build_dir" && presmoke_run_command "./$package")
    presmoke_repair_custom_op_package_headers "$build_dir"
    if ! presmoke_custom_op_package_valid; then
        echo "custom_op package install finished but required files are still incomplete" >&2
        return 1
    fi
    touch "$stamp"
}

presmoke_cmake() {
    local build_dir="$1"
    shift
    mkdir -p "$build_dir"
    (
        cd "$build_dir"
        presmoke_run_command cmake "$@" -DCMAKE_ASC_ARCHITECTURES="$(presmoke_arch)"
    )
}

presmoke_make() {
    local build_dir="$1"
    (
        cd "$build_dir"
        presmoke_run_command make -j
    )
}
