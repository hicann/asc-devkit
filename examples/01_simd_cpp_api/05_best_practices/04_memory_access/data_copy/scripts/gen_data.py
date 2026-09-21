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

import argparse
import os

import numpy as np


ARCH_ADDR_CONFLICT_SHAPE = {"dav-2201": (6144, 512), "dav-3510": (8192, 512)}

SCENARIO_CONFIG = {
    "UB": {
        1: (12288, 12288),
        2: (12288, 12288),
        3: (12288, 12288),
        4: (12288, 12287),
        5: (12288, 12288),
        6: (12288, 12288),
        7: (6144, 512),
        8: (6144, 512),
    },
    "L1": {
        1: (12288, 12288),
        2: (12288, 12288),
        3: (12288, 12287),
        4: (12288, 12288),
        5: (12288, 12288),
        6: (6144, 512),
        7: (6144, 512),
    },
}


def get_shape(copy_dst, scenario_num, arch):
    try:
        if (copy_dst == "UB" and scenario_num in (7, 8)) or (copy_dst == "L1" and scenario_num in (6, 7)):
            return ARCH_ADDR_CONFLICT_SHAPE[arch]
        return SCENARIO_CONFIG[copy_dst][scenario_num]
    except KeyError as exc:
        raise ValueError(f"Unsupported {copy_dst} scenario: {scenario_num}") from exc


def gen_golden_data(scenario_num, copy_dst, arch):
    m, n = get_shape(copy_dst, scenario_num, arch)
    element_count = m * n

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    rng = np.random.default_rng(0)
    input_data = rng.integers(-2, 3, size=element_count, dtype=np.int16).astype(np.float16)
    input_data.tofile("./input/input.bin")

    if copy_dst == "UB":
        input_data.tofile("./output/golden.bin")
        print(f"[INFO] Generated UB sample with shape [{m}, {n}]")
        return

    for path in ("./output/output.bin", "./output/golden.bin"):
        if os.path.exists(path):
            os.remove(path)
    print(f"[INFO] Generated L1 sample with shape [{m}, {n}]")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1)
    parser.add_argument("-copyDst", type=str, default="UB", choices=["UB", "L1"])
    parser.add_argument("-arch", type=str, default="dav-2201", choices=["dav-2201", "dav-3510"])
    args = parser.parse_args()

    gen_golden_data(args.scenarioNum, args.copyDst, args.arch)
