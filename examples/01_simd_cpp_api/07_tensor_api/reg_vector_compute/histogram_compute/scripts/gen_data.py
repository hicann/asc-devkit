#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the license.

from pathlib import Path

import numpy as np


input_length = 256
initial_value = np.uint16(3)


def main() -> None:
    Path("input").mkdir(exist_ok=True)
    Path("output").mkdir(exist_ok=True)

    indexes = np.arange(input_length, dtype=np.uint16)
    data = (indexes * np.uint16(37) + indexes % np.uint16(11)).astype(np.uint8)
    frequency = np.bincount(data, minlength=256).astype(np.uint16)
    golden = (frequency[:128] + initial_value).astype(np.uint16)

    data.tofile(Path("input") / "input.bin")
    golden.tofile(Path("output") / "golden.bin")
    print("Generated frequency histogram data for low half bins.")


if __name__ == "__main__":
    main()
