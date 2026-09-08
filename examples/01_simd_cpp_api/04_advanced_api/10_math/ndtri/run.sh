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

# 用法: bash run.sh -r <cpu|sim|npu> --size <N> --is_perf <0|1> [--api_mode <1|2|3>]
#   -r         运行模式：cpu（CPU调试）、sim（NPU仿真）、npu（NPU上板）
#   --size     计算量，即 TOTAL_LENGTH，默认 2048
#   --is_perf  0 功能验证；1 性能验证
#   --api_mode 1 指定calCount；2 整Tensor重载；3 整Tensor重载完全同址原地计算，默认1

set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

RUN_MODE=""
SIZE=""
IS_PERF=""
API_MODE="1"

while [[ $# -gt 0 ]]; do
    case "$1" in
        -r)
            RUN_MODE="$2"; shift 2;;
        --size)
            SIZE="$2"; shift 2;;
        --is_perf)
            IS_PERF="$2"; shift 2;;
        --api_mode)
            API_MODE="$2"; shift 2;;
        *)
            echo "[ERROR] Unexpected option: $1"; exit 1;;
    esac
done

# 必传参数校验
if [ -z "$RUN_MODE" ] || [ -z "$SIZE" ] || [ -z "$IS_PERF" ]; then
    echo "[ERROR] Missing required arguments. Usage: bash run.sh -r <cpu|sim|npu> --size <N> --is_perf <0|1>"
    [ -z "$RUN_MODE" ] && echo "  - -r (RUN_MODE) is required: cpu | sim | npu"
    [ -z "$SIZE" ]     && echo "  - --size (SIZE) is required: a positive integer"
    [ -z "$IS_PERF" ]  && echo "  - --is_perf is required: 0 (功能验证) | 1 (性能验证)"
    exit 1
fi

# 取值校验
if [ "${RUN_MODE}" != "cpu" ] && [ "${RUN_MODE}" != "sim" ] && [ "${RUN_MODE}" != "npu" ]; then
    echo "[ERROR] RUN_MODE(-r) must be cpu | sim | npu, got: ${RUN_MODE}"
    exit 1
fi
if ! [[ "${SIZE}" =~ ^[0-9]+$ ]] || [ "${SIZE}" -le 0 ]; then
    echo "[ERROR] --size must be a positive integer, got: ${SIZE}"
    exit 1
fi
if [ "${IS_PERF}" != "0" ] && [ "${IS_PERF}" != "1" ]; then
    echo "[ERROR] --is_perf must be 0 | 1, got: ${IS_PERF}"
    exit 1
fi
if [ "${API_MODE}" != "1" ] && [ "${API_MODE}" != "2" ] && [ "${API_MODE}" != "3" ]; then
    echo "[ERROR] --api_mode must be 1 | 2 | 3, got: ${API_MODE}"
    exit 1
fi
if [ "${IS_PERF}" = "1" ] && [ "${API_MODE}" != "1" ]; then
    echo "[ERROR] 性能验证固定使用指定calCount重载，请设置--api_mode 1"
    exit 1
fi

# 组合校验：性能验证仅 npu 模式支持（需 msProf 采集 AIV_VEC 数据）
if [ "${IS_PERF}" = "1" ] && [ "${RUN_MODE}" != "npu" ]; then
    echo "[ERROR] 性能验证(--is_perf 1)仅支持 npu 模式，请使用 -r npu 运行以采集 msProf 性能数据"
    exit 1
fi

ARCH="dav-3510"

build_and_run() {
    local test_mode=$1
    # 清理 build 目录，但保留 msprof 生成的 PROF_* 性能数据目录
    if [ -d build ]; then
        find build -mindepth 1 -maxdepth 1 ! -name 'PROF_*' -exec rm -rf {} +
    fi
    mkdir -p build
    cd build

    # 编译选项说明：
    # | 选项                   | 可选值                       | 说明                                                                                  |
    # |------------------------|------------------------------|---------------------------------------------------------------------------------------|
    # | CMAKE_ASC_RUN_MODE     | npu（默认）、cpu、sim        | 运行模式：NPU 运行、CPU调试、NPU仿真                                                 |
    # | CMAKE_ASC_ARCHITECTURES| dav-3510                     | NPU 架构：dav-3510 对应 Ascend 950PR                                                 |
    # | TEST_MODE              | 1（默认）、2、3              | 验证模式：功能验证、空跑基线、计算循环1000次                                         |
    # | SIZE                   | 正整数（默认 2048）          | 计算量，即 TOTAL_LENGTH                                                               |
    cmake "-DCMAKE_ASC_RUN_MODE=${RUN_MODE}" "-DCMAKE_ASC_ARCHITECTURES=${ARCH}" \
          "-DTEST_MODE=${test_mode}" "-DAPI_MODE=${API_MODE}" "-DSIZE=${SIZE}" \
          "-DASC_DEVKIT_PATH=${ASC_DEVKIT_PATH:-}" ..
    cmake --build . --parallel

    python3 ../scripts/gen_data.py ${SIZE} ${test_mode}

    # 功能验证(test_mode=1)直接运行；性能验证(test_mode=2/3)通过 msprof 采集，生成 op_summary_*.csv
    if [ "${test_mode}" = "1" ]; then
        ./demo
    elif [ "${RUN_MODE}" = "npu" ]; then
        msprof --application=./demo --output=./
    elif [ "${RUN_MODE}" = "sim" ]; then
        msprof op simulator --application=./demo
    elif [ "${RUN_MODE}" = "cpu" ]; then
        ./demo
    fi

    cd ..
}

# 从最近一次 msprof 采集结果 op_summary_*.csv 中提取指定字段值（单位 μs）
# 用法: extract_perf_field <field_name>
# 字段名（表头）大小写不敏感，返回最新 op_summary 的首行数值
extract_perf_field() {
    local field="$1"
    local csv
    # 查 build 目录下最新的 op_summary_*.csv（按修改时间倒序，兼容历史 PROF_* 目录保留场景）
    csv=$(find ./build -type f -name "op_summary_*.csv" -printf '%T@ %p\n' 2>/dev/null | sort -rn | head -1 | cut -d' ' -f2-)
    if [ -z "$csv" ] || [ ! -f "$csv" ]; then
        echo "[ERROR] op_summary_*.csv not found under ./build" >&2
        echo "FAIL"
        return 1
    fi
    # 解析：第1行为表头，第2行为数据，按逗号分隔；字段名可能带 (μs) 后缀，统一转小写后按前缀匹配
    python3 - "$csv" "$field" <<'PYEOF'
import csv, sys
path, field = sys.argv[1], sys.argv[2].lower()
with open(path, newline='', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        for k, v in row.items():
            if k and k.strip().lower().startswith(field):
                try:
                    print(float(v))
                except ValueError:
                    print(v.strip())
                sys.exit(0)
    print("FAIL", end="")
    sys.exit(1)
PYEOF
}

# 性能验证：采集 total1 / total2 / compute_time，计算 AIV_VEC 占比，校验是否达标（>=90%）
run_perf_verify() {
    echo "========== 性能验证 - 空跑基线 TEST_MODE=2 =========="
    build_and_run 2
    local total1
    total1=$(extract_perf_field "aiv_time")
    echo "[PERF] total1 (Aiv_time, TEST_MODE=2) = ${total1} us"

    echo "========== 性能验证 - 计算场景 TEST_MODE=3 =========="
    build_and_run 3
    local total2 compute_time
    total2=$(extract_perf_field "aiv_time")
    compute_time=$(extract_perf_field "aiv_vec_time")
    echo "[PERF] total2 (Aiv_time, TEST_MODE=3) = ${total2} us"
    echo "[PERF] compute_time (Aiv_vec_time, TEST_MODE=3) = ${compute_time} us"

    # 数值合法性校验
    if ! [[ "$total1" =~ ^[0-9.]+$ ]] || ! [[ "$total2" =~ ^[0-9.]+$ ]] || ! [[ "$compute_time" =~ ^[0-9.]+$ ]]; then
        echo "[ERROR] failed to extract valid perf data: total1=${total1}, total2=${total2}, compute_time=${compute_time}"
        exit 1
    fi

    # 计算 AIV_VEC 占比 = compute_time / (total2 - total1)
    local ratio
    ratio=$(python3 -c "t1=${total1}; t2=${total2}; c=${compute_time}; d=t2-t1; print('NaN' if d<=0 else round(c/d*100, 2))")
    echo "[PERF] AIV_VEC 占比 = compute_time / (total2 - total1) = ${compute_time} / (${total2} - ${total1}) = ${ratio}%"

    # 达标标准：占比 >= 90%
    local threshold=90
    local pass_fail="PASS"
    if [[ "$ratio" == "NaN" ]] || (( $(python3 -c "print(1 if float('${ratio}') < ${threshold} else 0)") )); then
        pass_fail="FAIL"
    fi
    echo "[PERF] 达标标准: 占比 >= ${threshold}%  =>  ${pass_fail}"
    if [ "$pass_fail" != "PASS" ]; then
        echo "[ERROR] 性能验证未达标"
        exit 1
    fi
    echo "性能验证达标，当前占比: ${ratio}%"
}

if [ "${IS_PERF}" = "0" ]; then
    echo "========== 功能验证 TEST_MODE=1 =========="
    build_and_run 1
else
    run_perf_verify
fi
