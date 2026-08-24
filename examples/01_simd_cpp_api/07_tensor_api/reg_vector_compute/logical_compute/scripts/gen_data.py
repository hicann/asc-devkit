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


vector_length = 64
shift_bits = 1


def write_case(name: str, data: np.ndarray) -> None:
    data.tofile(Path("input") / f"{name}_input.bin")
    np.left_shift(data, shift_bits).astype(data.dtype).tofile(Path("output") / f"{name}_golden.bin")


def main() -> None:
    Path("input").mkdir(exist_ok=True)
    Path("output").mkdir(exist_ok=True)

    indexes = np.arange(vector_length)
    write_case("int8", indexes.astype(np.int8))
    write_case("uint8", indexes.astype(np.uint8))
    write_case("int16", (indexes * 3).astype(np.int16))
    write_case("uint16", (indexes * 3).astype(np.uint16))
    write_case("int32", (indexes * 17).astype(np.int32))
    write_case("uint32", (indexes * 17).astype(np.uint32))
    print("Generated shift-left input and golden data for six integer types.")


if __name__ == "__main__":
    main()
