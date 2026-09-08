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

set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

RUN_MODE="${1:-npu}"
RUN_PERF="${2:-1}"

for size in 1 32 1023 2048 65536; do
    echo "========== 功能验证 size=${size} =========="
    bash run.sh -r "${RUN_MODE}" --size "${size}" --is_perf 0
done

echo "========== 整Tensor重载验证 size=1023 =========="
bash run.sh -r "${RUN_MODE}" --size 1023 --is_perf 0 --api_mode 2

echo "========== 整Tensor重载完全同址原地计算验证 size=1023 =========="
bash run.sh -r "${RUN_MODE}" --size 1023 --is_perf 0 --api_mode 3

if [ "${RUN_PERF}" = "1" ]; then
    if [ "${RUN_MODE}" != "npu" ]; then
        echo "[ERROR] 性能验证仅支持npu模式"
        exit 1
    fi
    for size in 1024 4096 8192 16384 32768 65536; do
        echo "========== 性能验证 size=${size} =========="
        bash run.sh -r npu --size "${size}" --is_perf 1
    done
fi
