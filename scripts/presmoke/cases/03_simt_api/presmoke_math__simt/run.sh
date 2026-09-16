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

CASE_REL=03_simt_api/presmoke_math__simt
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../../_case_entry.sh"
presmoke_case_init "$CASE_REL"

SOURCE_DIR="$PRESMOKE_PROJECT_ROOT/tests/api/simt_api/presmoke"
BUILD_DIR="$SOURCE_DIR/build_presmoke_simt"
export BUILD_DIR

case_build() {
    cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
        -DPRESMOKE_PROGRAMMING_MODE=simt
    cmake --build "$BUILD_DIR" -j"${PRESMOKE_MAKE_JOBS:-1}"
}

case_run() { :; }
case_verify() { :; }
case_clean() { rm -rf "$BUILD_DIR"; }

presmoke_case_main "$@"
