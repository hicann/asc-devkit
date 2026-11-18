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

set -euo pipefail

CASE_REL=01_simd_cpp_api/02_features/05_aclrtc/rtc_kernel_specialization
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../../../_case_entry.sh"
presmoke_case_init "$CASE_REL"

case_build() {
    local compute_unit
    case "${SOC_VERSION,,}" in
        ascend910b*) compute_unit=ascend910b ;;
        ascend910_93*) compute_unit=ascend910_93 ;;
        ascend950*) compute_unit=ascend950 ;;
        *) echo "Unsupported processor: $SOC_VERSION" >&2; return 2 ;;
    esac
    cmake -S "$CASE_DIR/custom_op" -B "$BUILD_DIR/operator" -DASCEND_COMPUTE_UNIT="$compute_unit"
    cmake --build "$BUILD_DIR/operator" --target binary package -j"${PRESMOKE_MAKE_JOBS:-8}"
    local packages=("$BUILD_DIR"/operator/custom_opp_*.run)
    if [[ ${#packages[@]} -ne 1 || ! -f "${packages[0]}" ]]; then
        echo "Expected one operator package in $BUILD_DIR/operator" >&2
        return 1
    fi
    mkdir -p "$BUILD_DIR/opp"
    bash "${packages[0]}" --quiet --install-path="$BUILD_DIR/opp"
    cmake -S "$CASE_DIR" -B "$BUILD_DIR/host"
    cmake --build "$BUILD_DIR/host" -j"${PRESMOKE_MAKE_JOBS:-8}"
}

case_run() {
    local vendor="$BUILD_DIR/opp/vendors/customize"
    export ASCEND_CUSTOM_OPP_PATH="${ASCEND_CUSTOM_OPP_PATH:-}"
    source "$vendor/bin/set_env.bash"
    export ASCEND_CUSTOM_OPP_PATH="$vendor"
    local manifest resource_id kernel_entry
    manifest=$(find "$BUILD_DIR/operator" -type f -name '*_manifest.json' -print -quit)
    if [[ -z "$manifest" ]]; then
        echo "No compilation resource Manifest found in $BUILD_DIR/operator" >&2
        return 1
    fi
    resource_id=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["resource_id"])' "$manifest")
    kernel_entry=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["kernels"][0]["kernel_name"])' "$manifest")
    "$BUILD_DIR/host/demo" "$resource_id" "$kernel_entry" "$BUILD_DIR/specialized.elf" \
        "${ASCEND_DEVICE_ID:-0}" | tee "$BUILD_DIR/result.log"
}

case_verify() {
    grep -Fq "Example passed" "$BUILD_DIR/result.log"
}

case_clean() {
    presmoke_default_clean
}

presmoke_case_main "$@"
