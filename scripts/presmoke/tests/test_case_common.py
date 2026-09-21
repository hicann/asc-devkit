#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from __future__ import annotations

import subprocess
import tempfile
import unittest
from pathlib import Path


class CaseCommonTest(unittest.TestCase):
    def test_concurrent_custom_op_package_install_is_serialized(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            custom_op = root / "examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
            custom_op.mkdir(parents=True)
            opp = root / "opp"
            state = root / "state"
            active = root / "active"
            collision = root / "collision"

            script = f"""
set -u
source {case_common}
export PRESMOKE_PROJECT_ROOT={root}
export PRESMOKE_STATE_DIR={state}
export PRESMOKE_LOCK_ROOT={root}/locks
export ASCEND_OPP_PATH={opp}
export PRESMOKE_ARCH=dav-2201
export PRESMOKE_MODE=npu
presmoke_run_command() {{
    if [[ "$1" == "make" ]]; then
        [[ "${{PRESMOKE_MAKE_JOBS:-}}" == "16" ]] || return 71
        [[ "${{CMAKE_BUILD_PARALLEL_LEVEL:-}}" == "16" ]] || return 72
        if ! mkdir {active} 2>/dev/null; then
            touch {collision}
            return 70
        fi
        sleep 0.5
        cat > custom_opp_test.run <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
vendor="$ASCEND_OPP_PATH/vendors/customize"
mkdir -p "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic"
mkdir -p "$vendor/op_api/include" "$vendor/op_api/lib"
mkdir -p "$vendor/framework/onnx" "$vendor/op_impl/ai_core/tbe/op_master_device/lib"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.py"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom_tiling_sink.py"
touch "$vendor/op_api/include/aclnn_add_custom.h"
touch "$vendor/op_api/include/aclnn_add_custom_tiling_sink.h"
touch "$vendor/op_api/lib/libcust_opapi.so"
touch "$vendor/framework/onnx/libcust_onnx_parsers.so"
touch "$vendor/op_impl/ai_core/tbe/op_master_device/lib/libcust_opmaster.so"
RUN
        chmod +x custom_opp_test.run
        rmdir {active}
        return 0
    fi
    if [[ "$1" == "./custom_opp_test.run" ]]; then
        "$@"
    fi
}}
presmoke_ensure_custom_op_package &
first=$!
presmoke_ensure_custom_op_package &
second=$!
wait "$first"; first_rc=$?
wait "$second"; second_rc=$?
[[ "$first_rc" -eq 0 && "$second_rc" -eq 0 && ! -e {collision} ]]
"""

            result = subprocess.run(["bash", "-c", script], text=True, capture_output=True, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertFalse(collision.exists())


if __name__ == "__main__":
    unittest.main()
