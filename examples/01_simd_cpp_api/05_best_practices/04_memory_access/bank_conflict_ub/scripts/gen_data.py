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

import numpy as np
import os
import argparse

# ---- kernel 常量 (与 bank_conflict_ub.asc / config.h 对齐) ----
TOTAL_LENGTH = 4096
REPEAT_TIMES_FOR_INSTR_EXCUTE = 1000
ITEMS_PER_ITER = 64  # 256 bytes / sizeof(float) = 64 elements per repeat
REPEAT_TIMES = TOTAL_LENGTH // ITEMS_PER_ITER  # 64
DATABLOCK_FLOATS = 8  # 32 bytes / sizeof(float) = 8 floats per DataBlock
NUM_BLOCKS_PER_REPEAT = ITEMS_PER_ITER // DATABLOCK_FLOATS  # 8

# ---- BinaryRepeatParams: (dstBlkStride, src0BlkStride, src1BlkStride, dstRepStride, src0RepStride, src1RepStride) ----
STRIDE_CONFIGS = {
    1: (1, 1, 1, 8, 8, 8),
    2: (1, 1, 1, 8, 8, 8),
    3: (1, 1, 1, 8, 8, 8),
    4: (1, 1, 1, 8, 8, 8),
    5: (1, 1, 1, 8, 8, 8),
    6: (1, 1, 1, 8, 8, 8),
    7: (1, 0, 1, 8, 1, 8),
    8: (1, 0, 1, 8, 8, 8),
}


def _build_src0_pattern(x_data, scenario_num):
    """根据 Add 高维切分语义和输入的BinaryRepeatParams，构造一次 Add 调用中 src0 的读取结果。

    对 repeat r (0..63)、DataBlock k (0..7)：
      src0 起始地址 = x_off + r * src0RepStride * 8 + k * src0BlkStride * 8
      连续读取 8 个 float 元素
    """
    _, src0BlkS, _, _, src0RepS, _ = STRIDE_CONFIGS[scenario_num]
    src0 = np.empty(TOTAL_LENGTH, dtype=np.float32)
    for r in range(REPEAT_TIMES):
        for k in range(NUM_BLOCKS_PER_REPEAT):
            src_idx = r * src0RepS * DATABLOCK_FLOATS + k * src0BlkS * DATABLOCK_FLOATS
            dst_start = r * ITEMS_PER_ITER + k * DATABLOCK_FLOATS
            src0[dst_start : dst_start + DATABLOCK_FLOATS] = x_data[src_idx : src_idx + DATABLOCK_FLOATS]
    return src0


def _gen_golden_no_overlap(x_data, y_data):
    return (x_data + y_data).astype(np.float32)


def _gen_golden_z_eq_y(x_data, y_data, scenario_num):
    src0_pattern = _build_src0_pattern(x_data, scenario_num)
    result = y_data.copy()
    for _ in range(REPEAT_TIMES_FOR_INSTR_EXCUTE):
        result += src0_pattern
    return result


def gen_golden_data(scenario_num):
    np.random.seed(0)
    input_x = np.random.uniform(1, 100, [TOTAL_LENGTH]).astype(np.float32)
    input_y = np.random.uniform(1, 100, [TOTAL_LENGTH]).astype(np.float32)

    if scenario_num in (1, 2, 3, 4, 5):
        # x, y, z无地址重叠
        golden = _gen_golden_no_overlap(input_x, input_y)
    else:
        # y和z地址完全重叠
        golden = _gen_golden_z_eq_y(input_x, input_y, scenario_num)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")
    golden.tofile("./output/golden.bin")

    print(f"[INFO] SCENARIO_NUM={scenario_num}, golden generated")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate test data for bank_conflict_ub")
    parser.add_argument(
        "--scenario", "-s", type=int, default=None, help="scenario number (1-8), overrides SCENARIO_NUM env"
    )
    args = parser.parse_args()

    scenario_num = args.scenario if args.scenario is not None else int(os.environ.get("SCENARIO_NUM", "1"))
    if scenario_num not in STRIDE_CONFIGS:
        print(f"[ERROR] Invalid scenario_num={scenario_num}. Valid range: 1-8")
        exit(1)
    gen_golden_data(scenario_num)
