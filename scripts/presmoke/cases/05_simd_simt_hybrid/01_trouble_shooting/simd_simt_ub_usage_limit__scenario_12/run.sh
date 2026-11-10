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

CASE_REL=05_simd_simt_hybrid/01_trouble_shooting/simd_simt_ub_usage_limit
SKIP_REASON='troubleshooting sample: this scenario fails by design (see the scenario table in README.md); skipped by presmoke'

SKIP_MODES=(npu sim)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../../_case_entry.sh"
presmoke_case_init "$CASE_REL"
BUILD_DIR="$CASE_DIR/build_${MODE}_scenario_12"
export BUILD_DIR

case_build() {
    mkdir -p "$BUILD_DIR"
    :
}

case_run() {
    mkdir -p "$BUILD_DIR"
    :
}

case_verify() {
    mkdir -p "$BUILD_DIR"
    :
}

case_clean() {
    presmoke_default_clean
}

presmoke_case_main "$@"
