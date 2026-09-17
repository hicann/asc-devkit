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


def gen_golden_data(scenario_num):
    buffer_count = 68
    copied_count = 64

    input_x = np.arange(1000, 1000 + buffer_count, dtype=np.uint64)
    golden = np.zeros(buffer_count, dtype=np.uint64)
    golden[1 : copied_count + 1] = input_x[1 : copied_count + 1]

    os.makedirs("input", exist_ok=True)
    os.makedirs("output", exist_ok=True)
    input_x.tofile("./input/input_x.bin")
    golden.tofile("./output/golden.bin")
    print(f"generated scenario {scenario_num}: copied_count={copied_count}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-scenarioNum",
        type=int,
        default=1,
        choices=[1, 2, 3, 4],
        help=(
            "Scenario number: 1=RegTraitNumOne Store without count, "
            "2=RegTraitNumOne Store with count, 3=RegTraitNumTwo Store without count, "
            "4=RegTraitNumTwo Store with count"
        ),
    )
    args = parser.parse_args()
    gen_golden_data(args.scenarioNum)
