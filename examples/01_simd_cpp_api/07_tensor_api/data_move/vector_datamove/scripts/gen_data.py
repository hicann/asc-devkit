#!/usr/bin/python3
# coding=utf-8

# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# Licensed under CANN Open Software License Agreement Version 2.0.

import os

import numpy as np


TOTAL_LENGTH = 1024


def gen_golden_data():
    data = ((np.arange(TOTAL_LENGTH, dtype=np.int16) * 13 + 7) % 251 - 125).astype(np.int8)

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    data.tofile("input/input_x.bin")
    data.tofile("output/golden.bin")


if __name__ == "__main__":
    gen_golden_data()
