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

from pathlib import Path

import numpy as np


ELEMENT_COUNT = 512
ALPHA = np.float16(0.01)


def main():
    input_dir = Path("input")
    output_dir = Path("output")
    input_dir.mkdir(exist_ok=True)
    output_dir.mkdir(exist_ok=True)

    input_data = ((np.arange(ELEMENT_COUNT, dtype=np.int32) % 97) - 48).astype(
        np.float16
    )
    golden = np.where(input_data > 0, input_data, input_data * ALPHA).astype(np.float16)
    input_data.tofile(input_dir / "input_x.bin")
    golden.tofile(output_dir / "golden.bin")


if __name__ == "__main__":
    main()
