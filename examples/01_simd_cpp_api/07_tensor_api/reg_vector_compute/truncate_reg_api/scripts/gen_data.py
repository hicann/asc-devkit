#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# Licensed under CANN Open Software License Agreement Version 2.0.

import argparse
import os

import numpy as np


TOTAL_LENGTH = 256


def float32_to_bfloat16(values):
    """Convert float32 values to bfloat16 bit patterns using round-to-nearest-even."""
    bits = values.astype(np.float32).view(np.uint32)
    rounding_bias = np.uint32(0x7FFF) + ((bits >> np.uint32(16)) & np.uint32(1))
    return ((bits + rounding_bias) >> np.uint32(16)).astype(np.uint16)


def bfloat16_to_float32(values):
    return (values.astype(np.uint32) << np.uint32(16)).view(np.float32)


def gen_golden_data(scenario_num):
    rng = np.random.default_rng(42)
    values = rng.uniform(-100.0, 100.0, TOTAL_LENGTH).astype(np.float32)

    # Include exact integers and values close to zero to demonstrate truncation semantics.
    values[:8] = np.array([-3.9, -2.0, -1.1, -0.9, 0.9, 1.1, 2.0, 3.9], dtype=np.float32)

    if scenario_num == 1:
        x = values.astype(np.float16)
        truncated = np.trunc(x)
        golden = np.where(truncated == 0, 0.0, truncated).astype(np.float16)
    elif scenario_num == 2:
        x = float32_to_bfloat16(values)
        truncated = np.trunc(bfloat16_to_float32(x))
        golden = float32_to_bfloat16(np.where(truncated == 0, 0.0, truncated))
    else:
        x = values
        truncated = np.trunc(x)
        golden = np.where(truncated == 0, 0.0, truncated).astype(np.float32)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("input/input_x.bin")
    golden.tofile("output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2, 3])
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
