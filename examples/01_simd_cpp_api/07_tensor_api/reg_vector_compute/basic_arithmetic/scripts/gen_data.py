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


def write_case(name: str, data: np.ndarray) -> None:
    data.tofile(Path("input") / f"{name}_input.bin")
    np.abs(data).astype(data.dtype).tofile(Path("output") / f"{name}_golden.bin")


def main() -> None:
    Path("input").mkdir(exist_ok=True)
    Path("output").mkdir(exist_ok=True)

    indexes = np.arange(vector_length)
    write_case("int8", (indexes.astype(np.int16) - 32).astype(np.int8))
    write_case("int16", (indexes.astype(np.int32) * 3 - 96).astype(np.int16))
    write_case("half", np.linspace(-2.0, 2.0, vector_length, dtype=np.float16))
    write_case("int32", (indexes.astype(np.int64) * 11 - 320).astype(np.int32))
    write_case("float", np.linspace(-3.0, 3.0, vector_length, dtype=np.float32))
    print("Generated abs input and golden data for int8, int16, half, int32, and float.")


if __name__ == "__main__":
    main()
