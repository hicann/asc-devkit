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


def write_case(name: str, dtype: type) -> None:
    input_dir = Path("input")
    output_dir = Path("output")
    input_a = np.linspace(-1.0, 1.0, vector_length, dtype=dtype)
    input_b = np.linspace(0.25, 1.25, vector_length, dtype=dtype)
    input_c = np.linspace(-0.5, 0.5, vector_length, dtype=dtype)
    golden = (input_a * input_b + input_c).astype(dtype)
    input_a.tofile(input_dir / f"{name}_input_a.bin")
    input_b.tofile(input_dir / f"{name}_input_b.bin")
    input_c.tofile(input_dir / f"{name}_input_c.bin")
    golden.tofile(output_dir / f"{name}_golden.bin")


def main() -> None:
    Path("input").mkdir(exist_ok=True)
    Path("output").mkdir(exist_ok=True)

    write_case("half", np.float16)
    write_case("float", np.float32)
    print("Generated fma input and golden data for half and float.")


if __name__ == "__main__":
    main()
