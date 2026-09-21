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


import os
import argparse
import numpy as np


def gen_golden_data(scenario_num):
    input_length = 1024
    data_type = np.float16  # half类型

    # 生成输入数据：两个[1, 1024]向量
    input_x = np.arange(input_length, dtype=data_type).reshape(1, input_length)
    input_y = np.arange(input_length, dtype=data_type).reshape(1, input_length)

    # 创建目录
    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)

    # 保存输入数据
    input_x.tofile("./input/input_x.bin")
    input_y.tofile("./input/input_y.bin")

    # 根据场景计算golden数据和输出长度（z = x + y）
    output_length = 1024
    z = input_x + input_y  # 计算z = x + y

    if scenario_num == 1:
        # 场景1：使用开发者自定义的迭代间偏移，计算1021个元素，不足1024的置零
        output_length = 1024
        golden = z[0, :output_length].reshape(1, output_length)
        golden[0, output_length - 3 : output_length] = 0
    elif scenario_num == 2:
        # 场景2：使用PostUpdate模式表示迭代间偏移，全量搬出
        output_length = 1024
        golden = z.copy()
    elif scenario_num == 3:
        # 场景3：使用地址寄存器（AddrReg）表示迭代间偏移，全量搬出
        output_length = 1024
        golden = z.copy()
    elif scenario_num == 4:
        # 场景4：以DataBlock为单位非连续搬运，32B间隔搬出
        output_length = 512
        golden = z.reshape(input_length // 32, 32)[:, 0:16].reshape(1, output_length)
    elif scenario_num == 5:
        # 场景5：广播（broadcast）模式搬入，每256B置为第一个元素
        output_length = 1024
        golden = z.copy()
        for i in range(output_length):
            golden[0, i] = golden[0, i // 128 * 128]
    elif scenario_num == 6:
        # 场景6：上采样（upsample）模式搬入，每个元素重复一次
        output_length = 2048
        golden = np.array([z, z]).transpose().reshape(1, output_length)
    else:
        raise ValueError(f"不支持的场景编号: {scenario_num}")

    # 保存golden数据
    golden.tofile("./output/golden.bin")
    print(f"生成完成: input_length={input_length}, output_length={output_length}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-scenarioNum", type=int, default=1, choices=[1, 2, 3, 4, 5, 6], help="Scenario number: 1-6")
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
