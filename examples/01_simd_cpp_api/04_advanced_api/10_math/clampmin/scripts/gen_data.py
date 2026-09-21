#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


import os
import numpy as np
import argparse


def gen_golden_data_simple(clamp_type):
    # 初始化参数
    dtype = np.float32
    data_size = 256
    src_shape = [data_size]
    min_num = -65536
    max_num = 65535
    scalar = 10
    # 产生随机的输入数据
    src = np.random.uniform(min_num, max_num, src_shape).astype(dtype)
    # 生成 golden 数据
    golden = src[:]
    if clamp_type == 0:
        golden = np.clip(src, a_min=scalar, a_max=None)
    else:
        golden = np.clip(src, a_min=None, a_max=scalar)
    # 保存数据文件
    os.makedirs("input", exist_ok=True)
    src.tofile("./input/input_x.bin")
    os.makedirs("output", exist_ok=True)
    golden.tofile("./output/golden.bin")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenario", type=int, choices=(0, 1), help="指定clampmin或者clampmax，取值0或1")
    args = parser.parse_args()
    gen_golden_data_simple(args.scenario if args.scenario else 0)
