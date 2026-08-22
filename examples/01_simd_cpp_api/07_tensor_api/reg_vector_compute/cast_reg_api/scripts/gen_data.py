#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# Licensed under CANN Open Software License Agreement Version 2.0.

import argparse
import os

import numpy as np


def gen_golden_data(scenario_num):
    total_length = 256
    np.random.seed(42)

    if scenario_num == 1:
        x = np.random.uniform(-100, 100, total_length).astype(np.float16)
        golden = np.floor(x).astype(np.int32)
    elif scenario_num == 2:
        x = np.random.uniform(-100, 100, total_length).astype(np.float32)
        golden = np.clip(np.rint(x), np.iinfo(np.int16).min,
                         np.iinfo(np.int16).max).astype(np.int16)
    elif scenario_num == 3:
        x = np.random.randint(-128, 128, total_length).astype(np.int8)
        golden = x.astype(np.int32)
    elif scenario_num == 4:
        x = np.random.randint(-128, 384, total_length).astype(np.int32)
        golden = np.clip(x, 0, 255).astype(np.uint8)
    elif scenario_num == 5:
        x_f32 = np.random.uniform(-100, 100, total_length).astype(np.float32)
        x = (x_f32.view(np.uint32) >> 16).astype(np.uint16)
        golden = (x.astype(np.uint32) << 16).view(np.float32)
    else:
        x = np.random.uniform(-100, 100, total_length).astype(np.float32)
        bits = x.view(np.uint32)
        upper = bits >> np.uint32(16)
        lower = bits & np.uint32(0xFFFF)
        carry = ((lower > np.uint32(0x8000)) |
                 ((lower == np.uint32(0x8000)) & ((upper & np.uint32(1)) != 0)))
        golden = (upper + carry.astype(np.uint32)).astype(np.uint16)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    x.tofile("input/input_x.bin")
    golden.tofile("output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1,
                        choices=[1, 2, 3, 4, 5, 6])
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
